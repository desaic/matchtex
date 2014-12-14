#include "Sudoku.hpp"
#include <iostream>
#include <fstream>
int main(){
  Sudoku sudoku;
  const char * filename = "sudoku.txt";
  std::ifstream in("sudoku.txt");
  if(!in.good()){
    std::cout<<"Cannot open "<<filename<<"\n";
    return -1;
  }
  sudoku.load(in);
  sudoku.print(std::cout);
  sudoku.solve();
  sudoku.print(std::cout);
  system("pause");
  return 0;
}