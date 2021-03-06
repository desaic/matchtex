/*************************************************************************
 *                                                                       *
 * "sparseMatrix" library , Copyright (C) 2007 CMU, 2009 MIT             *
 * All rights reserved.                                                  *
 *                                                                       *
 * Code author: Jernej Barbic                                            *
 * http://www.jernejbarbic.com/code                                      *
 * Research: Jernej Barbic, Doug L. James, Jovan Popovic                 *
 * Funding: NSF, Link Foundation, Singapore-MIT GAMBIT Game Lab          *
 * Version 2.0                                                           *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the BSD-style license that is            *
 * included with this library in the file LICENSE.txt                    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the file     *
 * LICENSE.TXT for more details.                                         *
 *                                                                       *
 *************************************************************************/

#ifndef _SPARSE_MATRIX_H_
#define _SPARSE_MATRIX_H_

/*
  The "SparseMatrix" class implements double-precision sparse matrices 
  with common algebraic operations such as incremental construction, 
  addition, mtx-vec multiplication, row-column deletion. 
  The matrices can be loaded from and saved to a file.

  The matrix can be rectangular (it need not be necessarily square). 
  In memory, the matrix is stored in a row-based format. 
  For each matrix row, the class stores the integer
  indices of the columns containing non-zero entries, 
  together with the corresponding double precision values. 
  All quantities (rows, columns, etc.) in this class are 0-indexed.

  Also included is a Conjugate Gradient iterative linear system solver 
  (for positive-definite large sparse symmetric matrices).
  The solver can be used without preconditioning, or with diagonal (Jacobi) preconditoning.
  The solver either uses an explicitly provided sparse matrix, or you can only
  provide the matrix-vector multiplication routine (without explicitly giving the matrix).
  The CG Solver was implemented by following Jonathan Shewchuk's 
  "An Introduction to the Conjugate Gradient Method Without the Agonizing Pain":
  http://www.cs.cmu.edu/~jrs/jrspapers.html#cg

  The class also includes a Gauss-Seidel iterative linear system solver.

  There are two classes available to the user: SparseMatrixOutline and SparseMatrix.
  The SparseMatrixOutline class should be used to construct the non-zero 
  locations in the matrix, and (optionally) assign values to them.
  You should then transform it into a SparseMatrix class, via SparseMatrix's
  class constructor. The SparseMatrix class has all the computational routines,
  but you can only add new non-zero entries to SparseMatrixOutline, not
  to SparseMatrix.  The reason for this separation is that SparseMatrixOutline 
  does not know the number of matrix entries ahead of time, so it uses STL's 
  "map" datastructure to store the matrix data. In the SparseMatrix class, however, the number 
  of sparse entries and their locations are fixed, so all operations can use 
  known-length C arrays, which is faster.

  So: you should first create an instance of SparseMatrixOutline, then create 
  an instance of SparseMatrix by passing the SparseMatrixOutline object to 
  SparseMatrix's constructor.
  If your matrix is a text file on disk, you can load it to SparseMatrixOutline, 
  or directly load it into SparseMatrix (which will, however, internally still 
  proceed via SparseMatrixOutline).

  The text disk file format is as follows:
  <number of matrix rows>
  <number of matrix columns>
  one or more data lines, each giving one matrix entry, in the format:
  <row index> <column index> <data value> 
  (indices are 0-indexed)

  Example:
  
    [0 17 -1 0]
  A=[0  5  0 0]
    [3  8  6 0]  

  would be given as:

  3
  4
  0 1 17 
  0 2 -1
  1 1 5
  2 0 3
  2 1 8
  2 2 6

  See also example.cpp for example usage.
*/

#ifdef WIN32
  #pragma warning(disable: 4786)
#endif

#include <vector>
#include <map>

class SparseMatrixOutline
{
public:
  // makes an empty sparse matrix with n rows
  SparseMatrixOutline(int n);
  ~SparseMatrixOutline();

  // makes a diagonal n x n sparse matrix; with a constant diagonal
  SparseMatrixOutline(int n, double diagonal);
  // makes a diagonal n x n sparse matrix; diagonal is a vector of n numbers
  SparseMatrixOutline(int n, double * diagonal);

  // loads the sparse matrix from a text file
  // if expand is greater than 1, the routine also expands each element into a diagonal block of size expand x expand... 
  //   (expand option is useful for loading the mass matrix in structural mechanics (with expand=3 in 3D))
  SparseMatrixOutline(char * filename, int expand=1); 

  // save matrix to a text file
  int Save(char * filename, int oneIndexed=0) const;

  // add entry at location (i,j) in the matrix
  void AddEntry(int i, int j, double value=0.0);
  void AddBlock3x3Entry(int i, int j, double * matrix3x3); // matrix3x3 should be given in row-major order
  void MultiplyRow(int row, double scalar); // multiplies all elements in row 'row' with scalar 'scalar'

  inline int Getn() const { return n_; } // get number of rows
  inline int GetNumRows() const { return n_; } // get number of rows
  int GetNumColumns() const; // get the number of columns (i.e., search for max column index)
  int GetNumEntries() const; // get total number of non-zero matrix elements
  double GetEntry(int i, int j) const; // returns the matrix entry at location (i,j) in the matrix (or zero if entry has not been assigned)
  void Print() const;

  // low-level routine which is rarely used
  inline const std::map<int,double> & GetRow(int i) const { return entries_[i]; }
  friend class SparseMatrix;

protected:
  int n_;
  std::vector< std::map<int,double> > entries_;

  void MakeEmpty();
};

class SparseMatrix
{
public:

  SparseMatrix(char * filename); // load from text file (same text file format as SparseMatrixOutline)
  SparseMatrix(SparseMatrixOutline * sparseMatrixOutline); // create it from the outline
  SparseMatrix(const SparseMatrix & source); // copy constructor
  ~SparseMatrix();

  int Save(char * filename, int oneIndexed=0) const; // save matrix to a disk text file 

  int SaveToMatlabFormat(char * filename) const; // save matrix to a text file that can be imported into Matlab

  // set/add value to j-th non-zero element in row 'row' (not to matrix element at (row,j) !)
  inline void SetEntry(int row, int j, double value) { centries_[row][j] = value; }
  inline void AddEntry(int row, int j, double value) { centries_[row][j] += value; }
  void ResetToZero(); // reset all entries to zero

  inline int Getn() const { return n_; } // get the number of rows
  inline int GetNumRows() const { return n_; }
  inline int GetRowLength(int row) const { return rowLength_[row]; }
  int GetNumColumns() const; // get the number of columns (i.e., search for max column index)
  // return the j-th non-zero element in row i (not matrix element at (row, j) !)
  inline double GetEntry(int row, int j) const { return centries_[row][j]; }
  // returns the column index of the j-th non-zero element in row 'row'
  inline int GetColumnIndex(int row, int j) const { return cindices_[row][j]; } 
  // finds the position (in row 'row') of element with column index jAbsolute
  // returns -1 if column not found
  int GetInverseIndex(int row, int jAbsolute) const;

  int GetNumEntries() const; // returns the total number of non-zero entries
  double SumEntries() const; // returns the sum of all matrix entries
  void SumRowEntries(double * rowSums) const; // returns the sum of all entries in each row
  double GetMaxAbsEntry() const; // max abs value of a matrix entry
  double GetInfinityNorm() const; // matrix infinity norm
  void Print(int sparsePrint=0) const; // prints the matrix out to standard output

  // matrix algebra (all involved matrices must have the same pattern of non-zero entries)
  SparseMatrix operator+(const SparseMatrix & mat2) const;
  SparseMatrix operator-(const SparseMatrix & mat2) const;
  friend SparseMatrix operator* (const double alpha, const SparseMatrix & mat2); // warning: this function makes a local copy; "ScalarMultiply" is more efficient
  SparseMatrix & operator=(const SparseMatrix & source); // matrices must have same size and locations of non-zero entries
  SparseMatrix & operator*=(const double alpha);
  SparseMatrix & operator+=(const SparseMatrix & mat2);
  SparseMatrix & operator-=(const SparseMatrix & mat2);
  void ScalarMultiply(const double alpha, SparseMatrix * dest=NULL); // dest := alpha * dest (if dest=NULL, operation is applied to this object)
  void MultiplyRow(int row, double scalar); // multiplies all elements in row 'row' with scalar 'scalar'

  // multiplies the sparse matrix with the given vector
  void MultiplyVector(const double * input, double * result) const; // result = A * input
  void MultiplyVectorAdd(const double * input, double * result) const; // result += A * input
  // multiplies the transpose of the matrix with the given vector
  void TransposeMultiplyVector(const double * input, double * result, int resultLength) const; // result = Transpose(A) * input
  void TransposeMultiplyVectorAdd(const double * input, double * result) const; // result += Transpose(A) * input
  void MultiplyMatrix(int numRows, int numColumns, const double * input, double * result) const; // result = A * input (input is a numRows x numColumns dense matrix, result is a n_ x numColumns dense matrix
  void MultiplyMatrixAdd(const double * input, int numColumns, double * result) const; // result += A * input (result and input are now dense matrices with 'numColumns' columns

  // computes <M*input, input> (assumes symmetric M)
  double QuadraticForm(const double * input) const;
  // normalizes vector in the M-norm: vector := vector / sqrt(<M*vector, vector>)  (assumes symmetric M)
  void NormalizeVector(double * vector) const;
  SparseMatrix ConjugateMatrix (SparseMatrix & S); // computes S^T M S (M can be a general square matrix, S need not be a square matrix; sizes of M and S must be such that product is defined)

  // writes all entries into the space provided by 'data'
  // space must be pre-allocated
  // data is written row after row, and by non-zero columns within each row
  void MakeLinearDataArray(double * data) const;
  // writes row indices of non-zero entries into array "indices"
  // same order as for data
  void MakeLinearRowIndexArray(int * indices) const;
  // indices in this function version are double to ensure compatibility with Matlab
  void MakeLinearRowIndexArray(double * indices) const;
  // writes column indices
  void MakeLinearColumnIndexArray(int * indices) const;
  void MakeLinearColumnIndexArray(double * indices) const;

  // make a dense matrix (column-major LAPACK style storage)
  // (this can be a huge matrix for large sparse matrices)
  // storage in denseMatrix must be pre-allocated
  void MakeDenseMatrix(double * denseMatrix) const;

  // removes row and column 'index' from the matrix
  void RemoveRowColumn(int index); // 0-indexed
  void RemoveRowsColumns(int numRowColumns, int * rowColumns, int oneIndexed=0); // the rowColumns must be sorted (ascending)
  void RemoveRowsColumnsSlow(int numRowColumns, int * rowColumns, int oneIndexed=0); // the rowColumns need not be sorted
  void RemoveColumn(int index); // 0-indexed
  void RemoveColumns(int numColumns, int * columns, int oneIndexed=0); // 0-indexed; columns must be sorted (ascending)
  void IncreaseNumRows(int newNumRows); // increases the number of matrix rows (new rows are all zero)
  void SetRows(SparseMatrix * source, int startRow, int startColumn=0); // starting with startRow, overwrites the rows with those of matrix 'source'; data is written into columns starting at startColumn

  // transposition (note: the matrix need not be symmetric)
  void BuildTranspositionIndices();
  void FreeTranspositionIndices();
  // returns the list position of the transposed element (row, list position j)
  inline int TransposedIndex(int row, int j) const { return transposeIndex_[row][j]; }

  // returns the transposed matrix
  SparseMatrix Transpose();

  // checks if the matrix is skew-symmetric
  // the non-zero entry locations must form a symmetric pattern
  // returns max ( abs ( A^T + A ) ) = || A^T + A ||_{\infty}
  double SkewSymmetricCheck(); 
  // makes matrix symmetric by copying upper triangle + diagonal into the lower triangle
  // the non-zero entry locations must form a symmetric pattern
  void SymmetrizeMatrix();

  // builds the positions of diagonal elements in each row
  // this routine will accelerate subsequent GetDiagonal or AddDiagonalMatrix calls, but is not necessary for GetDiagonal or AddDiagonalMatrix
  void BuildDiagonalIndices();
  void FreeDiagonalIndices();
  void GetDiagonal(double * diagonal);
  void AddDiagonalMatrix(double * diagonalMatrix);

  // add a matrix to the current matrix, whose elements are a subset of the elements of the current matrix
  // call this once to establish the correspondence
  void BuildSubMatrixIndices(SparseMatrix & mat2);
  // += factor * mat2
  // returns *this
  SparseMatrix & AddSubMatrix(double factor, SparseMatrix & mat2);

  // copy data from a matrix into a submatrix obtained by a previous call to RemoveRowColumns
  // oneIndexed: tells whether the fixed rows and columns are specified 1-indexed or 0-indexed
  // First, call BuildSuperMatrixIndices once to inialize (all fixed rows and columns are indexed with respect the superMatrix):
  void BuildSuperMatrixIndices(int numFixedRowColumns, int * fixedRowColumns, SparseMatrix * superMatrix, int oneIndexed=0); // use this version if the indices of removed rows and columns are the same
  void BuildSuperMatrixIndices(int numFixedRows, int * fixedRows, int numFixedColumns, int * fixedColumns, SparseMatrix * superMatrix, int oneIndexed=0); // allows arbitrary row and column indices
  // Then, call this (potentially many times) to quickly assign the values at the appropriate places in the submatrix:
  void AssignSuperMatrix(SparseMatrix * superMatrix);

  // returns the total number of non-zero entries in the lower triangle (including diagonal)
  int GetNumLowerTriangleEntries() const;
  int GetNumUpperTriangleEntries() const;
  // exports the matrix to format for NAG library
  int GenerateNAGFormat(double * a,int * irow,int * icol, int * istr) const;

  void GenerateCompressedRowMajorFormat(double * a, int * ia, int * ja, int upperTriangleOnly=0, int oneIndexed=0) const; 
  void GenerateCompressedRowMajorFormat_four_array(double * values, int * columns, int * pointerB, int * pointerE, int upperTriangleOnly=0, int oneIndexed=0) const; 

  // diagonal solve M * x = b
  // ASSUMES the sparse matrix is diagonal !
  // result is overwritten into rhs
  // (to solve non-diagonal linear systems, you need to use an external library; or you can use the CGSolver class included with this sparse matrix package, or you can use the Gauss-Seidel iteration below)
  void DiagonalSolve(double * rhs) const;

  // performs one Gauss-Seidel iteration of solving the system A * x = b
  // updates vector x in place, b is not modified
  // (A can be a general matrix)
  // assumes that diagonal entries of the matrix are set and are non-zero
  void DoOneGaussSeidelIteration(double * x, const double * b) const;
  void ComputeResidual(const double * x, const double * b, double * residual) const;
  // checks if A * x - b is close to zero and prints out the findings
  // returns ||A * x - b|| / ||b|| (all norms are infinity)
  // passing a buffer (length of n) will avoid a malloc/free pair to generate scratch space for the residual
  double CheckLinearSystemSolution(const double * x, const double * b, int verbose=1, double * buffer=NULL) const;

  // below are low-level routines which are rarely used
  inline double ** GetDataHandle() const { return centries_; }
  inline double * GetRowHandle(int row) const { return centries_[row]; }

protected:

  // compact representation for fast matrix multiplications
  int n_;
  int * rowLength_;
  int ** cindices_;
  double ** centries_;

  int * diagonalIndices_;
  int ** transposeIndex_;

  int ** subMatrixIndices_;
  int * subMatrixIndexLengths_;

  int ** superMatrixIndices_;
  int * superRows_;

  void InitFromOutline(SparseMatrixOutline * sparseMatrixOutline);
  void Allocate();
  void BuildRenumberingVector(int nConstrained, int nSuper, int numFixedDOFs, int * fixedDOFs, int ** superDOFs, int oneIndexed=0);
};

#endif

