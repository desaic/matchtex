#include "voxel.hpp"
#include "trigAABBIntersect.hpp"
#include <set>
#include <GL/gl.h>
void bbox(int ii, Mesh & m , real_t * mn, real_t gridlen, int * tmn, int * tmx);

void Voxel::draw()
{
  std::set<GridIdx>::iterator it;
  glBegin(GL_TRIANGLES);
  real_t gridlen = 1.0/gridsize;
  for(it = gridset.begin();it!=gridset.end();it++){
    for(size_t  ii=0;ii<cube.t.size();ii++){
      for(int jj=0;jj<3;jj++){
        Vec3 v = cube.v[cube.t[ii][jj]];
        Vec3 n = cube.n[cube.t[ii][jj]];
        glNormal3f(n[0],n[1],n[2]);
        glVertex3f(v[0]+ gridlen * (*it)[0]-0.5,
                   v[1]+ gridlen * (*it)[1]-0.5,
                   v[2]+ gridlen * (*it)[2]-0.5);
      }
    }

  }
  glEnd();
}

bool trigCubeIntersect(int tidx, const Mesh & m ,
                       GridIdx & cube,
                       real_t * mn, real_t side_len)
{
  float boxcenter[3]={mn[0]+(0.5+cube[0])*side_len,
                      mn[1]+(0.5+cube[1])*side_len,
                      mn[2]+(0.5+cube[2])*side_len};
  float boxhalfsize[3]={side_len/2,side_len/2,side_len/2};
  float triverts[3][3];
  for(int ii=0;ii<3;ii++){
    for(int jj=0;jj<3;jj++){
      triverts[ii][jj]=m.v[m.t[tidx][ii]][jj];
    }
  }
  return triBoxOverlap(boxcenter,boxhalfsize, triverts);
}

void vec2grid(Vec3 & v, real_t * mn, real_t gridlen, GridIdx & grid)
{
  for(int ii=0; ii<VOXEL_DIM; ii++) {
    grid[ii]= (int)((v[ii]-mn[ii])/gridlen);
  }
}

void rasterize(int tidx, Mesh & m , real_t * mn, real_t gridlen, std::set<GridIdx> & gridset)
{
  //bounding box of a triangle
  int tmn[3], tmx[3];
  bbox(tidx,m,mn,gridlen,tmn,tmx);
  for(int ix=tmn[0];ix<=tmx[0];ix++){
    for(int iy=tmn[1];iy<=tmx[1];iy++){
      for(int iz=tmn[2];iz<=tmx[2];iz++){
        GridIdx grid(ix,iy,iz);
        if(trigCubeIntersect(tidx,m,grid,mn,gridlen)){
          gridset.insert(grid);
        }
      }
    }
  }
}

void bbox(int ii, Mesh & m , real_t * mn, real_t gridlen, int * tmn, int * tmx)
{
  GridIdx vidx;
  vec2grid(m.v[m.t[ii][0]],mn,gridlen,vidx);
  for(int jj=0; jj<3; jj++) {
    tmn[jj]=vidx[jj];
    tmx[jj]=vidx[jj];
  }
  for(int jj=0; jj<3; jj++) {
    vec2grid(m.v[m.t[ii][jj]],mn,gridlen,vidx);
    for(int kk=0; kk<VOXEL_DIM; kk++) {
      if(vidx[kk]<tmn[kk]) {
        tmn[kk]=vidx[kk];
      }
      if(vidx[kk]>tmx[kk]) {
        tmx[jj]=vidx[kk];
      }
    }
  }
}
Mesh Voxel::cube;
Voxel::Voxel(Mesh & m, int res):
gridsize(res)
{
  real_t mn[VOXEL_DIM], mx[VOXEL_DIM];
  real_t msize=0; //size of the longest axis of the bounding box of the mesh

  for(int jj=0; jj<VOXEL_DIM; jj++) {
    mn[jj]=m.v[0][jj];
    mx[jj]=m.v[0][jj];
  }

  for(size_t ii=0; ii<m.v.size(); ii++) {
    for(int jj=0; jj<VOXEL_DIM; jj++) {
      if(m.v[ii][jj]<mn[jj]) {
        mn[jj]=m.v[ii][jj];
      }
      if(m.v[ii][jj]>mx[jj]) {
        mx[jj]=m.v[ii][jj];
      }
    }
  }

  msize=mx[0]-mn[0];
  for(size_t jj=0; jj<VOXEL_DIM; jj++) {
    real_t len=mx[jj]-mn[jj];
    if(msize<len) {
      msize=len;
    }
  }

  for(size_t jj=0; jj<VOXEL_DIM; jj++) {
    mn[jj]=(mn[jj]+mx[jj]-msize)/2;
  }

  real_t gridlen=msize/res;

  for(size_t ii=0; ii<m.t.size(); ii++) {
    rasterize(ii,m,mn,gridlen,gridset);
  }

  if(cube.v.size()==0){
    Voxel::cube.load_mesh("cube.ply2");
  }
  for(size_t ii=0;ii<cube.v.size();ii++){
    cube.v[ii]/=gridsize;
  }
}
