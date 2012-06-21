#include "tetra.hpp"
#include <string>
#include <fstream>
#include <iostream>
Tetra::Tetra(const char * prefix)
{
  std::string eleSfx=".ele";
  std::string faceSfx=".face";
  std::string nodeSfx=".node";
  std::string filename = eleSfx.insert(0,prefix);
  std::ifstream in(filename.c_str());
  if(!in.good()){
    std::cout<<"tetra cannot open "<<filename<<"\n";
    return;
  }
  int n_ele;
  in>>n_ele;
  int dummy;
  in>>dummy;
  in>>dummy;
  e.resize(n_ele);
  for(int ii=0;ii<n_ele;ii++){
    int idx;
    in>>idx;
    for(int jj=0;jj<4;jj++){
      in>>e[ii][jj];
      e[ii][jj]--;
    }
  }
  in.close();
  filename = faceSfx.insert(0,prefix);
  in.open(filename.c_str());
  if(!in.good()){
    std::cout<<"tetra face cannot open "<<filename;
    return;
  }
  int n_face;
  in>>n_face;
  int nextra=0;
  in>>nextra;
  t.resize(n_face);
  for(int ii=0;ii<n_face;ii++){
    int idx;
    in>>idx;
    for(int jj=0;jj<3;jj++){
      in>>t[ii][jj];
      t[ii][jj]--;
    }
    for(int jj=0;jj<nextra;jj++){
      in>>dummy;
    }
  }
  in.close();

  filename = nodeSfx.insert(0,prefix);
  in.open(filename.c_str());
  int n_vert ;
  in >> n_vert;
  in>>dummy;
  in>>dummy;
  in>>dummy;
  v.resize(n_vert);
  for(int ii=0;ii<n_vert;ii++){
     int idx;
     in>>idx;
     for(int jj=0;jj<3;jj++){
        in >> v[ii][jj];
     }
  }
  in.close();

  rescale();
}

void Tetra::rescale()
{
  real_t mn[3]= {1,1,1};
  real_t mx[3]= {-1,-1,-1};

  //scale and translate to [0 , 1]
  for (unsigned int dim = 0; dim<3; dim++) {
    for( size_t ii=0; ii<v.size(); ii++) {
      mn [dim]= std::min(v[ii][dim],mn[dim]);
      mx[dim] = std::max(v[ii][dim],mx[dim]);
    }
    real_t translate = -0.5*(mx[dim]+mn[dim]);
    //  translate = -mn[dim];
    for(size_t ii=0; ii<v.size(); ii++) {
      v[ii][dim]=(v[ii][dim]+translate);
    }
  }

  real_t scale = 1/(mx[0]-mn[0]);
  for(unsigned int dim=1; dim<3; dim++) {
    scale=std::min(1/(mx[dim]-mn[dim]),scale);
  }

  for(size_t ii=0; ii<v.size(); ii++) {
    for (unsigned int dim = 0; dim<3; dim++) {
      v[ii][dim]=v[ii][dim]*scale;
    }
  }
}

void Tetra::draw( )
{
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glColor3f(0.8,0.8,0.8);
  glUseProgramObjectARB(0);
  glBegin(GL_LINES);

  for(unsigned int ii=0; ii<e.size(); ii++) {
    for(int jj=0;jj<3;jj++){
      for(int kk=jj+1;kk<4;kk++){
        glVertex3f(v[e[ii][jj]][0],v[e[ii][jj]][1],v[e[ii][jj]][2]);
        glVertex3f(v[e[ii][kk]][0],v[e[ii][kk]][1],v[e[ii][kk]][2]);
      }
    }
  }
  glEnd();
}
