#include <vector>

struct Move{
  int ii,jj,val;
  Move(int _ii, int _jj, int _val):ii(_ii),jj(_jj),val(_val){}
  Move():ii(0),jj(0),val(0){}
};

class Sudoku
{
  //board size 
  static const int bsize=9;
public:
  Sudoku();
  int board [bsize][bsize];
  bool cand [bsize][bsize][bsize+1];
  void whichCell(int ii, int jj, int * cc);
  void cellIdx(const int * cc, int kk, int * ij);
  void load(std::istream & in);
  //eliminate candidates based on board[ii][jj]
  bool updateCand(int ii, int jj);

  //@return false if move eliminates all candidates of any undetermined cell.
  bool applyMove(Move m);

  Move findMove();
  bool fin();
  bool solve();

  Sudoku& operator= (const Sudoku &sudoku);

  void print(std::ostream & out);
};