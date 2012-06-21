#ifndef VOXEL_HPP
#define VOXEL_HPP
#include "vec.h"
#include "mesh.hpp"
#include <vector>
#include <set>
#include "mesh.hpp"
#define VOXEL_DIM 3

struct GridIdx{
  int i[3];
  GridIdx(int a=0,int b=0,int c=0){
    i[0]=a;
    i[1]=b;
    i[2]=c;
  }
  int & operator[](int idx){return i[idx];}
  int operator[](int idx)const{return i[idx];}

  bool operator<(const GridIdx & grid)const{
    for(int ii=0;ii<VOXEL_DIM;ii++){
      if(i[ii]==grid.i[ii]){
        continue;
      }
      if(i[ii]<grid.i[ii]){
        return true;
      }
      return false;
    }
    return false;
  }
};

class Voxel{
public:
  Voxel(Mesh & m, int res = 50);
  int gridsize;
  std::set<GridIdx> gridset;
  void draw();
private:
  static Mesh cube;
};
#endif
