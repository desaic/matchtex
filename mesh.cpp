#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <cstdlib>
#include <utility>
#include "mesh_query.h"
#include <map>
#include <sstream>
#include <string.h>
#include "imageio.h"
#include "util.h"
bool contains(int * a ,  int x, size_t size)
{
  for(size_t ii=0; ii<size; ii++) {
    if(a[ii]==x) {
      return true;
    }
  }
  return false;
}

void Mesh::save(const char * filename)
{
  std::ofstream out;
  out.open(filename);
  out<<v.size()<<"\n"<<t.size()<<"\n";
  for(size_t ii=0; ii<v.size(); ii++) {
    for(int jj=0; jj<3; jj++) {
      out<<v[ii][jj]<<" ";
    }
    out<<"\n";
  }
  for(size_t ii=0; ii<t.size(); ii++) {
    out<<"3";
    for(int jj=0; jj<3; jj++) {
      out<<" "<<t[ii][jj];
    }
    out<<" "<<t[ii].label;
    out<<"\n";
  }
  out.close();
}

bool Mesh::self_intersect()
{ double * vertex = new double[3*v.size()];
  int * trig = new int[t.size()*3];
  int idx=0;
  for(size_t ii=0; ii<v.size(); ii++) {
    for(int jj=0; jj<3; jj++) {
      vertex[idx]=v[ii][jj];
      idx++;
    }
  }
  idx=0;
  for(size_t ii=0; ii<t.size(); ii++) {
    for(int jj=0; jj<3; jj++) {
      trig[idx]=t[ii][jj];
      idx++;
    }
  }
  MeshObject * mo = construct_mesh_object( v.size() , vertex , t.size(), trig);
  std::map<EdgeId, bool> m;
  double point0[3];
  double point1[3];
  int triangle_index;
  bool ret = false;
  double ss, tt, aa, bb, cc;
  for(size_t ii=0; ii<t.size(); ii++) {
    for(int jj=0; jj<3; jj++) {
      int a = t[ii][jj];
      int b = t[ii][(jj+1)%3];
      for(int kk=0; kk<3; kk++) {
        point0[kk]=v[a][kk];
        point1[kk]=v[b][kk];
      }
      EdgeId e(a,b);
      if(m[e]) {
        continue;
      }
      if(segment_intersects_mesh(point0,point1, mo, &triangle_index,&ss,&tt,&aa,&bb,&cc)) {
        printf("%d %d int trig %lu intersects trig %d\n",a,b,ii, triangle_index);
        bad[ii]=true;
        bad[triangle_index]=true;
        ret=true;
      }
    }
  }
  destroy_mesh_object(mo);
  return ret;
}

void Mesh::get_normal_center()
{
  real_t max_area=0;
  for (unsigned int ii=0; ii<t.size(); ii++) {
    Trig & tt= t[ii];
    Vec3 b=v[tt[2]]-v[tt[0]];
    tt.n=(v[tt[1]]-v[tt[0]]).cross(b);
    tt.A=tt.n.norm();
	//if(tt.A<0.000001){
//		std::cout<<"bad trig\n";
	//}
    if(tt.A>max_area){
      max_area=tt.A;
    }
    tt.n/=tt.n.norm();
    for (int ii=0; ii<3; ii++) {
      tt.c+=v[tt[ii]];
    }
    tt.c/=3;
  }
  //scale area so that maxmum area is 1
  for(size_t ii=0;ii<t.size();ii++){
    t[ii].A/=max_area;
  }
}

int findEdge(int va, int vb, std::vector<Trig>& t,
              std::vector<std::vector< int > > &vtlist, int * wings)
{
  int cnt=0;
  for(size_t ii=0; ii<vtlist[va].size(); ii++) {
    int tIdx = vtlist[va][ii];
    for(int jj=0; jj<3; jj++) {
      if(t[tIdx][jj]==vb) {
        wings[cnt]=tIdx;
        cnt++;
        if(cnt==2) {
          return cnt ;
        }
      }
    }
  }
  return cnt;
}
void Mesh::load_tex(const char * filename) {
  int width,height;
  unsigned char * buf = imageio_load_image(filename, &width,&height);
  if(!buf) {
    return;
  }
  tex_buf=buf;
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1,&texture);
  glBindTexture( GL_TEXTURE_2D, texture );
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_LUMINANCE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               width,  height,  0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  printf("gl error %d\n",glGetError());
  printf("max size %d\n",GL_MAX_TEXTURE_SIZE);
}

void Mesh::read_obj(std::ifstream & f)
{
  std::string line;
  std::string vTok("v");
  std::string fTok("f");
  std::string texTok("vt");
  char bslash='/',space=' ';
  std::string tok;
  while(1) {
    std::getline(f,line);
    if(f.eof()) {
      break;
    }
    if(line.size()<3) {
      continue;
    }
    if(line.at(0)=='#') {
      continue;
    }
    std::stringstream ss(line);
    ss>>tok;
    if(tok==vTok) {
      Vec3 vec;
      ss>>vec[0]>>vec[1]>>vec[2];
      v.push_back(vec);
    } else if(tok==fTok) {
      if(line.find(bslash)!=std::string::npos) {
        std::replace(line.begin(),line.end(),bslash,space);
        std::stringstream facess(line);
        Trig trig;
        facess>>tok;
        for(int ii=0; ii<3; ii++) {
          facess>>trig[ii]>>trig.texId[ii];
          trig[ii]--;
          trig.texId[ii]--;
        }
        t.push_back(trig);
      } else {
        Trig trig;
        for(int ii=0; ii<3; ii++) {
          ss>>trig[ii];
          trig[ii]--;
          trig.texId[ii]=0;
        }
        t.push_back(trig);
      }
    } else if(tok==texTok) {
      Vec3 texcoord;
      ss>>texcoord[0];
      ss>>texcoord[1];
      tex.push_back(texcoord);
    }
  }
}

void Mesh::read_ply(std::ifstream & f)
{
  std::string line;
  std::string vertLine("element vertex");
  std::string faceLine("element face");
  std::string texLine("property float s");
  std::string endHeaderLine("end_header");
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(vertLine)) {
      break;
    }
  }
  std::string token;
  std::stringstream ss(line);
  ss>>token>>token;
  int nvert;
  ss>>nvert;
  bool hasTex=false;
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(faceLine)) {
      break;
    }
    if(std::string::npos!=line.find(texLine)) {
      hasTex=true;
    }
  }
  std::stringstream ss1(line);
  ss1>>token>>token;
  int nface;
  ss1>>nface;
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(endHeaderLine)) {
      break;
    }
  }

  v.resize(nvert);
  t.resize(nface);
  if(hasTex) {
    tex.resize(nvert);
  }
  for (int ii =0; ii<nvert; ii++) {
    for (int jj=0; jj<3; jj++) {
      f>>v[ii][jj];
    }
    if(hasTex) {
      for (int jj=0; jj<2; jj++) {
        f>>tex[ii][jj];
      }
      tex[ii][1]=1-tex[ii][1];;
    }
  }
  for (int ii =0; ii<nface; ii++) {
    int nidx;
    f>>nidx;
    t[ii].label=0;
    for (int jj=0; jj<3; jj++) {
      f>>t[ii][jj];
    }
  }
}

void Mesh::read_ply2(std::ifstream&f)
{
  int nvert, ntrig;
  f>>nvert;
  f>>ntrig;
  v.resize(nvert);
  t.resize(ntrig);
  for (int ii =0; ii<nvert; ii++) {
    for (int jj=0; jj<3; jj++) {
      f>>v[ii][jj];
    }
  }
  std::string line;
  std::getline(f,line);
  for (int ii =0; ii<ntrig; ii++) {
    int nidx;
    std::getline(f,line);
    std::stringstream ss(line);
    ss>>nidx;
    for (int jj=0; jj<3; jj++) {
      ss>>t[ii][jj];
    }
    if(ss.eof()) {
      t[ii].label=0;
    } else {
      ss>>t[ii].label;
      if(ss.fail()) {
        t[ii].label=0;
      }
    }
  }
}

void int2b(unsigned int x, GLubyte * b)
{
  for(int ii=0;ii<4;ii++){
    b[ii]=x&0xff;
    x=x>>8;
  }
}

unsigned int b2int(GLubyte * b)
{
  unsigned int x=0;
  for(int ii=3;ii>=0;ii--){
    x=x<<8;
    x+=b[ii];
  }
  return x;
}

void Mesh::drawCol()
{
  glDisable(GL_LIGHTING);
  glBegin(GL_TRIANGLES);
  for(unsigned int ii=0; ii<t.size(); ii++) {
    unsigned int l = t[ii].label;
    GLubyte b[4];
    int2b(l,b);
    glColor4ub(b[0],b[1],b[2],b[3]);
    glVertex3f(v[t[ii][0]][0],v[t[ii][0]][1],v[t[ii][0]][2]);
    glVertex3f(v[t[ii][1]][0],v[t[ii][1]][1],v[t[ii][1]][2]);
    glVertex3f(v[t[ii][2]][0],v[t[ii][2]][1],v[t[ii][2]][2]);

  }
  glEnd();
}

void Mesh::save_obj(const char * filename)
{
  std::ofstream out(filename);
  if(!out.good()){
    std::cout<<"cannot open output file"<<filename<<"\n";
    return;
  }
  std::string vTok("v");
  std::string fTok("f");
  std::string texTok("vt");
  char bslash='/';
  std::string tok;
  for(size_t ii=0;ii<v.size();ii++){
    out<<vTok<<" "<<v[ii][0]<<" "<<v[ii][1]<<" "<<v[ii][2]<<"\n";
  }
  if(tex.size()>0){
    for(size_t ii=0;ii<tex.size();ii++){
      out<<texTok<<" "<<tex[ii][0]<<" "<<tex[ii][1]<<"\n";
    }
    for(size_t ii=0;ii<t.size();ii++){
      out<<fTok<<" "<<t[ii][0]+1<<bslash<<t[ii].texId[0]+1<<" "
      <<t[ii][1]+1<<bslash<<t[ii].texId[1]+1<<" "
      <<t[ii][2]+1<<bslash<<t[ii].texId[2]+1<<"\n";
    }
  }else{
    for(size_t ii=0;ii<t.size();ii++){
      out<<fTok<<" "<<t[ii][0]+1<<" "<<t[ii][1]+1<<" "<<t[ii][2]+1<<"\n";
    }
  }

  out.close();
}

Mesh::Mesh(const char * filename)
  :fbo(0),prog(0),tex_buf(0)

{
  load_mesh(filename);
}


void Mesh::load_mesh(const char * filename)
{
  std::ifstream f ;
  f.open(filename);
  if(!f.is_open()) {
    std::cout<<"mesh cannot open "<<filename<<"\n";
    return;
  }
  switch(filename[strlen(filename)-1]) {
  case 'y':
    read_ply(f);
    break;
  case 'j':
    read_obj(f);
    break;
  default:
    read_ply2(f);
  }
  rescale();
  compute_norm();

  f.close();
}
void Mesh::rescale()
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
void Mesh::compute_norm()
{
  n.resize(v.size());
  for(unsigned int ii=0; ii<t.size(); ii++) {
    Vec3 a = v[t[ii][1]] - v[t[ii][0]];
    Vec3 b = v[t[ii][2]] - v[t[ii][0]];
    b=a.cross(b);
    for(int jj=0; jj<3; jj++) {
      n[t[ii][jj]]+=b;
      if(t[ii][jj]>=(int)n.size() || t[ii][jj]<0){
        std::cout<<ii<<" "<<jj<<" "<<t[ii][jj]<<" file error\n";
      }
    }
  }
  for(unsigned int ii=0; ii<v.size(); ii++) {
    n[ii]/= n[ii].norm();
  }
}

void Mesh::draw(std::vector<Vec3>&v)
{
  //glDisable(GL_LIGHTING);
//  glDisable(GL_TEXTURE_2D);
  glUseProgramObjectARB(prog);
  GLfloat specular[4]= {0.51f,0.51f,0.51f,1.0f};
  GLfloat ambient[4]= {0.3f,0.3f,0.3f,1.0f};
  GLfloat diffuse[4]= {1.0f,1.0f,1.0f,1.0f};

  glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
  glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);

  GLfloat s=10;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&s);
  if(tex_buf) {
    glBindTexture(GL_TEXTURE_2D,texture);
    GLint hasTex=glGetUniformLocation(prog, "hasTex");
    glUniform1i(hasTex,1);
  }
  glBegin(GL_TRIANGLES);

  for(unsigned int ii=0; ii<t.size(); ii++) {
    if(tex_buf && tex.size()>0) {
      glNormal3f(n[t[ii][0]][0],n[t[ii][0]][1],n[t[ii][0]][2]);
      glTexCoord2f(tex[t[ii].texId[0]][0],tex[t[ii].texId[0]][1]);
      glVertex3f(v[t[ii][0]][0],v[t[ii][0]][1],v[t[ii][0]][2]);

      glTexCoord2f(tex[t[ii].texId[1]][0],tex[t[ii].texId[1]][1]);
      glNormal3f(n[t[ii][1]][0],n[t[ii][1]][1],n[t[ii][1]][2]);
      glVertex3f(v[t[ii][1]][0],v[t[ii][1]][1],v[t[ii][1]][2]);

      glTexCoord2f(tex[t[ii].texId[2]][0],tex[t[ii].texId[2]][1]);
      glNormal3f(n[t[ii][2]][0],n[t[ii][2]][1],n[t[ii][2]][2]);
      glVertex3f(v[t[ii][2]][0],v[t[ii][2]][1],v[t[ii][2]][2]);
    } else {
      glNormal3f(n[t[ii][0]][0],n[t[ii][0]][1],n[t[ii][0]][2]);
      glVertex3f(v[t[ii][0]][0],v[t[ii][0]][1],v[t[ii][0]][2]);
      glNormal3f(n[t[ii][1]][0],n[t[ii][1]][1],n[t[ii][1]][2]);
      glVertex3f(v[t[ii][1]][0],v[t[ii][1]][1],v[t[ii][1]][2]);
      glNormal3f(n[t[ii][2]][0],n[t[ii][2]][1],n[t[ii][2]][2]);
      glVertex3f(v[t[ii][2]][0],v[t[ii][2]][1],v[t[ii][2]][2]);
    }
  }
  glEnd();
  if(tex_buf) {
    glBindTexture(GL_TEXTURE_2D,0);
    GLint hasTex=glGetUniformLocation(prog, "hasTex");
    glUniform1i(hasTex,0);
  }
}

bool is_nbr(Trig & a, Trig&b, int vert)
{
  for (int ii=0; ii<3; ii++) {

    int va=a[ii];
    if(va<=vert) {
      continue;
    }

    for (unsigned int jj=0; jj<3; jj++) {
      int vb=b[jj];
      if(vb<=vert) {
        continue;
      }
      if(va==vb) {
        return true;
      }
    }
  }
  return false;
}


void Mesh::adjlist()
{
  if(adjMat.size()==t.size()) {
    return;
  }
  std::vector<std::vector<int> >trigList;
  trigList.resize(v.size());
  for (unsigned int ii=0; ii<t.size(); ii++) {
    for (unsigned int jj=0; jj<3; jj++) {
      int vidx=t[ii][jj];
      trigList[vidx].push_back(ii);
    }
  }
  adjMat.resize(t.size());
  for (unsigned int ii=0; ii<v.size(); ii++) {
    int n_nbr=trigList[ii].size();
    for (int jj=0; jj<n_nbr; jj++) {
      int tj=trigList[ii][jj];
      for (int kk=(jj+1); kk<n_nbr; kk++) {
        int tk=trigList[ii][kk];
        if(is_nbr(t[tj],t[tk],ii)) {
          adjMat[tj].push_back(tk);
          adjMat[tk].push_back(tj);
        }

      }
    }
  }
}

GLcharARB * read_entire_file(const char * filename, int * len )
{
  FILE * file = fopen(filename, "r");
  if(!file){
    printf("cannot open shader %s\n", filename);
    return 0;
  }
  GLcharARB * buf=0;
  fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	buf = new GLcharARB[length+1];
  length = fread( buf, 1, length, file);
  buf[length]=0;
  *len=length;
  return buf;
}

void printInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                         &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void Mesh::load_shader(const char * vsfile, const char * fsfile)
{
  glewInit();
  if(!glCreateShaderObjectARB){
    return;
  }
	vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  int len =0;
  GLcharARB * buf = read_entire_file(vsfile, &len);
  glShaderSourceARB(vs, 1,  (const GLcharARB**) (&buf), 0);
  glCompileShaderARB(vs);
  delete [] buf;

  buf = read_entire_file(fsfile, &len);
  glShaderSourceARB(fs, 1,  (const GLcharARB**) (&buf), 0);
  glCompileShaderARB(fs);
  delete [] buf;

	printInfoLog(vs);
	printInfoLog(fs);
	prog = glCreateProgramObjectARB();
	glAttachObjectARB(prog,vs);
	glAttachObjectARB(prog,fs);

	glLinkProgramARB(prog);
	printInfoLog(prog);
  GLint hasTex=glGetUniformLocation(prog, "hasTex");
  glUniform1i(hasTex,0);

	glUseProgramObjectARB(prog);

}

void Mesh::init_select()
{
  int tex_wid=1280, tex_hig=720;
  glGenTextures(1, &fbot);
  glBindTexture(GL_TEXTURE_2D, fbot);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_wid, tex_hig, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  GLuint rboId;
  glGenRenderbuffers(1, &rboId);
  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                      tex_wid, tex_hig);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                       GL_TEXTURE_2D, fbot, 0);
  // attach the renderbuffer to depth attachment point
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                          GL_RENDERBUFFER, rboId);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
