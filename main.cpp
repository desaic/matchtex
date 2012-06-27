#ifdef _WIN32
#include <windows.h>
#endif
#include "mesh.hpp" //glew.h is included before other headers
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <iostream>
#include <string>

#include "poly.hpp"
#include "mesh_query.h"
#include"quat.h"
#include "cgd.hpp"
#include "tetra.hpp"
#include "voxel.hpp"
#include "imageio.h"

#include "rep/manager.hpp"
static Mesh * m=0;
Tetra * tet=0;
Voxel * vox=0;
Fab::Shape * shape=0;
static Quat rot;
struct Cam{
  Cam():rotx(0),roty(0){
    for (int ii=0;ii<3;ii++){
      eye[ii]=0.0;
      at[ii]=0.0;
    }
    eye[2]=2;
  }
  GLdouble eye[3];
  GLdouble at[3];
  float rotx,roty;
};
static Cam* cam;
void init(void)
{
  glClearColor (1, 1, 1, 0.0);
  // glShadeModel (GL_SMOOTH);
  //glShadeModel (GL_FLAT );
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA);
  // glEnable(GL_NORMALIZE);


  GLfloat white[]={1.0,1.0,1.0,1.0};
  GLfloat grey[]={0.3,0.3,0.3,1.0};

  glLightfv (GL_LIGHT1, GL_DIFFUSE, white);
  glLightfv (GL_LIGHT1, GL_SPECULAR, white);
  glLightfv (GL_LIGHT0, GL_AMBIENT, grey);

}

bool running=false;
void display(void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(cam->eye[0], cam->eye[1],cam->eye[2],
	    cam->at[0],cam->at[1], cam->at[2],
	    0.0, 1.0, 0.0);
  GLfloat position[] = { 2.0, 2, 2, 1.0 };
  GLfloat position1[] = { -1.0, -1, -1, 1.0 };

  glPushMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(0,0,0);
  glTranslatef(position[0],position[1],position[2]);
  glutWireCube(0.2);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(position1[0],position1[1],position1[2]);
  glutWireCube(0.2);
  glEnable(GL_LIGHTING);
  glPopMatrix();

  glLightfv (GL_LIGHT0, GL_POSITION, position);
  glLightfv (GL_LIGHT1, GL_POSITION, position1);

  Vec3 axis ;
  real_t angle;
  glPushMatrix();
  rot.to_angle_axis(axis,&angle);
  angle=angle*180/3.14159;
 // std::cout<<angle<<"\n";
  //std::cout<<axis[0]<<" "<<axis[1]<<" "<<axis[2]<<"\n";

  glRotatef(angle,axis[0],axis[1],axis[2]);
  if(shape){
    shape->draw();
  }
 // if(tet){
 //   tet->draw();
 // }else{
   // m->draw(m->v);
  // vox->draw();
//  }
 // glBindFramebuffer(GL_FRAMEBUFFER, m->fbo);
 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 // m->drawCol();
 // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glPopMatrix();

  GLfloat floorCol[4]={1,1,1,1};
  glMaterialfv(GL_FRONT,GL_SPECULAR, floorCol);
  glMaterialfv(GL_FRONT,GL_DIFFUSE, floorCol);
  GLfloat s=10;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&s);
  glBegin(GL_TRIANGLE_STRIP);
  glNormal3f(0,1,0);
  glVertex3f( -1.0f, -.5f, -1.0f );
  glVertex3f( -1.0f, -.5f, 1.0f );
  glVertex3f( 1.0f, -.5f, -1.0f );
  glVertex3f( 1.5f, -.5f,  1.0f );
  glEnd();
  glFlush ();
}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
}

GLdouble  norm(GLdouble * v)
{
  GLdouble sum=0;
  for(int ii=0;ii<3;ii++){
    GLdouble d=cam->eye[ii]-cam->at[ii];
    sum+=d*d;
  }
  sum=sqrt(sum);
  return sum;
}

void add(GLdouble * a, GLdouble * b,GLdouble * ans,GLdouble c)
{
  for(int ii=0;ii<3;ii++){
    ans[ii]=a[ii]+c*b[ii];
  }
}
void mul(GLdouble * a,GLdouble c)
{
  for(int ii=0;ii<3;ii++){
    a[ii]*=c;
  }
}
void keyboard(unsigned char key,int x, int y)
{
  switch(key){
  case 'w':
    cam->eye[2]-=0.1;
    break;
  case 's':
    cam->eye[2]+=0.1;
    break;
  case 'a':
    cam->eye[0]-=0.1;
    break;
  case 'd':
    cam->eye[0]+=0.1;
    break;
  }
  glutPostRedisplay();
}

int ldown;
int oldx,oldy;

void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state){
    case GLUT_DOWN:
      ldown=1;
      oldx=x;
      oldy=y;
      break;
    case GLUT_UP:
      ldown=0;
     // glBindFramebuffer(GL_FRAMEBUFFER,m->fbo);
     // glFlush();
     // unsigned char buf[4];
    //  glReadPixels(x, 720-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    //  glBindFramebuffer(GL_FRAMEBUFFER,0);
      break;
    }
    break;
  default:
    break;
  }
}
void motion (int x, int y)
{

  if(ldown){
    Quat q(Vec3(0,1,0),(x-oldx)/60.0);
    rot =q*rot;
    rot=rot.normalize();
    q=Quat(Vec3(1,0,0),(y-oldy)/60.0);
    rot =q*rot;
    rot=rot.normalize();
    oldx=x;
    oldy=y;
  }

}

void animate(int t)
{
  glutTimerFunc(60, animate, 0);

  glutPostRedisplay();

}

extern int minc_nlabel;
void* iterate(void* arg){
  return 0;
}
extern int main2(Fab::Shape ** shape);
int main(int argc, char** argv)
{
  if(argc<2){
    printf("%s options\n",argv[0]);
    exit(0);
  }
  int fake=1;
  glutInit(&fake, argv);
  bool run=false;
  const char * tex_file="";
  const char * tetPrefix="";
  const char * meshfile="";
  int gridsize=50;
  for(int ii=0;ii<argc;ii++){
    if(strcmp(argv[ii],"-r")==0){
      run=true;
    }
    if(strcmp(argv[ii],"-t")==0){
      ii++;
      tex_file=argv[ii];
    }
    if(strcmp(argv[ii],"-tet")==0){
      ii++;
      tetPrefix=argv[ii];
    }
    if(strcmp(argv[ii],"-m")==0){
      ii++;
      meshfile=argv[ii];
    }
    if(strcmp(argv[ii],"-grid")==0){
      ii++;
      gridsize=atoi(argv[ii]);
    }
  }

  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH|GLUT_ALPHA );
  glutInitWindowSize (1280 , 720);
  glutCreateWindow (argv[0]);
  init ();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(0.1, animate, 0);
  glewInit();

  srand(123456);
 /// m=new Mesh (meshfile);
  //vox=new Voxel(*m,gridsize);
 // m->init_select();
 // m->load_shader("shader/vert.glsl","shader/frag.glsl");
 // if(tetPrefix[0]){
  //  tet=new Tetra(tetPrefix);
  //  m->t=tet->t;
 //   m->v=tet->v;
   // m->rescale();
 //   m->compute_norm();
 // }

 // if(tex_file[0]){
 //   m->load_tex(tex_file);
 // }
  if(run){
    pthread_t thread;
    pthread_create(&thread, 0, iterate,(void*)m);
    pthread_detach(thread);
  }
  cam=new Cam();
  rot=Quat(Vec3(0,0,1),0);
  //rot=Quat(Vec3(
  //-0.682098 ,-0.501571 ,-0.532136),108.429*3.141592/180);
  ldown=0;
  main2(&shape);
  glutMainLoop();
  return 0;
}
