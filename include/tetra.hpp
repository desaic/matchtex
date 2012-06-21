#ifndef TETRA_HPP
#define TETRA_HPP
#include <vector>
#include "math.hpp"
#include "mesh.hpp"

struct Elem
{
  int v[4];
  Elem(int v0=0,int v1=0,int v2=0,int v3=0)
  {
    v[0]=v0;
    v[1]=v1;
    v[2]=v2;
    v[3]=v3;
  }
  int & operator[](size_t idx){
    return v[idx];
  }
  int operator[](size_t idx)const {
    return v[idx];
  }
};

class Tetra{
public:
  Tetra(const char * prefix=0);
  std::vector<Vec3> v;
  std::vector<Trig> t;
  std::vector<Elem> e;
  void draw();
private:
  void rescale();
};

#endif
