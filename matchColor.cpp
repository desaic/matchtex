#include "rep/manager.hpp"
#include "math.hpp"
#include "imageio.h"

#include <GL/gl.h>

#include <iostream>

using namespace Fab;

template<typename NUM_T>
inline NUM_T trim(NUM_T x, NUM_T lower, NUM_T upper)
{
  if(x<lower){
    x=lower;
  }
  if(x>upper){
    x=upper;
  }
  return x;
}

///@brief helper class that stores a texture and faciliates lookup
///assume 4byte RGBA image data
class Texture{
public:
  Texture();
  void load(const char * filename){
    tex = imageio_load_image(filename, &width, &height);
  }
  void operator()(int x, int y,  unsigned char * color){
    x = trim(x,0,width-1);
    y = trim(y,0,height-1);
    int idx =4*(y*width + x);
    for(int ii=0;ii<4;ii++){
      color[ii] = tex[ idx + ii ];
    }
  }
  ///@param x assumed to be between 0 and 1
  void operator()(float x, float y, unsigned char * color){
    int ix,iy;
    x=x*width;
    y=y*height;
    ix = (int)x;
    iy = (int)y;
    unsigned char pixels[4][4];
    operator()(ix  ,iy  ,pixels[0]);
    operator()(ix+1,iy  ,pixels[1]);
    operator()(ix  ,iy+1,pixels[2]);
    operator()(ix+1,iy+1,pixels[3]);
    float alpha = x-ix;
    float beta = y-iy;
    for(int ii=0;ii<4;ii++){
      color[ii] = (1-alpha)*(1-beta)*pixels[0][ii]
                +    alpha *(1-beta)*pixels[1][ii]
                + (1-alpha)*   beta *pixels[2][ii]
                +    alpha *   beta *pixels[3][ii];
    }
  }
  ~Texture()
  {
    delete tex;
  }
private:
  unsigned char * tex;
  int width , height;
};

Texture::Texture():tex(0),width(0),height(0)
{
}

int rgb2Gray(unsigned char*color)
{
  return (int)(0.299*color[0]+0.587*color[1]+0.114*color[2]);
}

///@brief a textured model
class TexturedShape:public Shape{
public:

  TexturedShape():Shape(),tex(0), res(100)
  {
    nVertex   = res*res;
    nTriangle = 2*(res-1)*(res-1);
    vertex   =   new Vertex[nVertex];
    triangle = new Triangle[nTriangle];
    normal.resize(nVertex);

    float step = 1.0/res;
    for(int ii=0;ii<res;ii++){
      for(int jj=0;jj<res;jj++){
        int index = idx(ii,jj);
        normal[index]=Vec3(0,0,1);
        vertex[index].position[0]=ii*step;
        vertex[index].position[1]=jj*step;
      }
    }

    for(int ii=0;ii<res-1;ii++){
      for(int jj=0;jj<res-1;jj++){
        int index = 2*(ii+jj*(res-1));
        triangle[index].vertex[0] = idx(ii,jj);
        triangle[index].vertex[1] = idx(ii+1,jj);
        triangle[index].vertex[2] = idx(ii,jj+1);

        index++;
        triangle[index].vertex[0] = idx(ii+1,jj);
        triangle[index].vertex[1] = idx(ii+1,jj+1);
        triangle[index].vertex[2] = idx(ii,jj+1);
      }
    }

  }

  ~TexturedShape()
  {
    delete []vertex;
    delete []triangle;
    delete tex;
  }
  inline int idx(int ii, int jj)
  {
    return ii+jj*res;
  }
  void setTex(Texture * _tex)
  {
    tex = _tex;
    float step = 1.0/res;
    unsigned char color[4];
    for(int ii=0;ii<res;ii++){
      for(int jj=0;jj<res;jj++){
        (*tex)(ii*step ,jj*step,color);
        vertex[idx(ii,jj)].position[2]=rgb2Gray(color)/255.0;
      }
    }
    computeNormal();
  }
  void computeNormal(){
    for(size_t ii=0;ii<nTriangle;ii++){
      Vec3 a(triangle[ii].vertex[0]);
      Vec3 b(triangle[ii].vertex[1]);
      Vec3 c(triangle[ii].vertex[2]);
      Vec3 n = (b-a).cross(c-a);
      for(int jj=0;jj<3;jj++){
        normal[triangle[ii].vertex[jj]] += n;
      }
    }

    for(size_t ii=0;ii<nVertex;ii++){
      normal[ii]/=normal[ii].norm();
    }
  }
  Texture * tex;
  std::vector<Vec3> normal;
  void draw();
  int res;
};

void TexturedShape::draw()
{
  glBegin(GL_TRIANGLES);
  for(size_t ii=0;ii<nTriangle;ii++){
    for(int jj=0;jj<3;jj++){
      int vidx = triangle[ii].vertex[jj];
      glNormal3f (normal[vidx][0],normal[vidx][1],normal[vidx][2]);
      glVertex3f (vertex[vidx].position[0],
                  vertex[vidx].position[1],
                  vertex[vidx].position[2]);
    }
  }
  glEnd();
}

class ImageGoal:public Goal{
public:
  Texture * tex;
};

class FloatState:public State{
public:
  FloatState(const FloatState & state){
    a=state.a;
  }
  FloatState(){}
  std::vector<float> a;
  size_t size()const{return a.size();};
  float & operator[](int idx){return a[idx];}
  float operator[](int idx)const{return a[idx];}
};

/////////////Below is input API

int main2(Shape ** _shape){
  Texture * tex = new Texture;
  tex->load( "../bunny_tex1.png");
  TexturedShape * shape = new TexturedShape();
  shape -> setTex(tex);
  *_shape = shape;


  return 0;
}
