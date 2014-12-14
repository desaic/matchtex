#include "Sudoku.hpp"
#include <iostream>

Sudoku::Sudoku()
{}

void Sudoku::whichCell(int ii, int jj, int * cc)
{
  cc[0] = ii/3;
  cc[1] = jj/3;

}

void Sudoku::cellIdx(const int * cc, int kk, int * ij)
{
  int oi = kk/3;
  int oj = kk%3;
  ij[0] = 3*cc[0] + oi;
  ij[1] = 3*cc[1] + oj;
}

bool Sudoku::fin()
{
  for(int ii = 0;ii<bsize;ii++){
    for(int jj = 0;jj<bsize;jj++){
      if(board[ii][jj]==0){
        return 0;
      }
    }
  }
  return true;
}

bool Sudoku::solve()
{
  if(fin()){
    return true; 
  }
  while(1){
    Move m = findMove();
    if(m.val == 0){
      //no more moves
      return false;
    }
    Sudoku scopy = *this;
    bool result = scopy.applyMove(m);
    if(!result){
      cand[m.ii][m.jj][m.val] = false;
      continue;
    }
    result = scopy.solve();
    if(result){
      *this = scopy;
      return true;
    }else{
      cand[m.ii][m.jj][m.val] = false;
    }
  }
  return false;
}

Move Sudoku::findMove()
{
  Move minMove(0,0,0);
  int minCand=9;
  for(int ii = 0;ii<bsize; ii++){
    for(int jj = 0;jj<bsize;jj++){
      if(board[ii][jj] != 0){
        continue;
      }
      int nCand = 0;
      int firstNum = 0;
      for(int kk = 1; kk<=bsize; kk++){
        if(cand[ii][jj][kk]){
          if(firstNum == 0){
            firstNum = kk;
          }
          nCand ++;
        }
      }
      if(minCand>nCand){
        minCand = nCand;
        minMove = Move(ii, jj, firstNum);
      }
    }
  }
  return minMove;
}

void Sudoku::load(std::istream & in)
{

  for(int ii = 0;ii<bsize; ii++){
    for(int jj = 0;jj<bsize;jj++){
      in>>board[ii][jj];
      cand[ii][jj][0]=false;
      bool canVal = (board[ii][jj] == 0);
      for(int kk = 1; kk<=bsize;kk++){
        cand[ii][jj][kk] = canVal;
      }
    }
  }
  for(int ii = 0;ii<bsize; ii++){
    for(int jj = 0;jj<bsize;jj++){
      updateCand(ii,jj);
    }
  }
}

int countTrue(bool * arr, int size){
  int cnt = 0;
  for(int ii = 0;ii<size;ii++){
    if(arr[ii] ==true){
      cnt ++;
    }
  }
  return cnt;
}

int firstTrue(bool * arr, int size){
  for(int ii = 0;ii<size;ii++){
    if(arr[ii] ==true){
      return ii;
    }
  }
  return 0;
}

bool Sudoku::updateCand(int ii, int jj)
{
  int num = board[ii][jj];
  if(num==0){
    return true;
  }
  int cc[2];
  int ij[2];
  whichCell(ii,jj,cc);
  for(int idx = 0;idx<bsize;idx++){
    //check row
    if(board[ii][idx] == 0 && cand[ii][idx][num]){
      cand[ii][idx][num] = false;
      int cnt = countTrue(cand[ii][idx], bsize+1); 
      if(cnt==0){
        return false;
      }
      if(cnt==1){
      //  board[ii][idx]= firstTrue(cand[ii][idx], bsize+1);
      }
    }
    //check col
    if(board[idx][jj] == 0 &&cand[idx][jj][num]){
      cand[idx][jj][num] = false;
      int cnt = countTrue(cand[idx][jj], bsize+1);
      if(cnt==0){
        return false;
      }
      if(cnt==1){
      //  board[idx][jj] = firstTrue(cand[idx][jj], bsize+1);
      }
    }
    cellIdx(cc,idx,ij);
    if(board[ij[0]][ij[1]] == 0 && cand[ij[0]][ij[1]][num]){
      cand[ij[0]][ij[1]][num]=false;
      int cnt = countTrue( cand[ij[0]][ij[1]], bsize+1);
      if(cnt==0){
        return false;
      }
      if(cnt==1){
      //  board[ij[0]][ij[1]]= firstTrue( cand[ij[0]][ij[1]], bsize+1);
      }
    }
  }
  return true;
}

bool Sudoku::applyMove(Move m)
{
  board[m.ii][m.jj] = m.val;
  return updateCand(m.ii,m.jj);
}

Sudoku& Sudoku::operator= (const Sudoku &ss)
{
  for(int ii = 0;ii<bsize; ii++){
    for(int jj = 0;jj<bsize;jj++){
      board[ii][jj]= ss.board[ii][jj];
      for(int kk = 0; kk<bsize;kk++){
        cand[ii][jj][kk] = ss.cand[ii][jj][kk];
      }
    }
  }
  return *this;
}

void Sudoku::print(std::ostream & out)
{
  int nRow = 3;
  for(int ii = 0;ii<bsize;ii++){
    for(int kk = 0;kk<nRow;kk++){
      for(int jj = 0;jj<bsize;jj++){
        int num = board[ii][jj];
        for(int ll = 0; ll<nRow; ll++){
          int cc = nRow * kk+ll+1;
          if(num != 0 && kk==1 && ll==1){
            out<<num<<" ";
          }else if(num == 0 && cand[ii][jj][cc]){
            out<<cc<<" ";
          }else{
            out<<"  ";
          }
        }
        out<<" | ";
      }
      out<<"\n";
    }
    out<<"-----------------------------------------------------\n";
  }

}
