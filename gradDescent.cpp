#include "rep/manager.hpp"
#include "math.hpp"
#include <iostream>
using namespace Fab;

/////////////
///user implemented modules
///@bug crazy pointer casts because I was too lazy to write templates
///will be fixed as soon as this version is working
////////////
static std::vector<Vec3> * path;
class Poly:public Shape{
public:
  Poly(){
    for(int ii=0;ii<4;ii++){
      coef[ii]=0;
    }
  }
  float coef[4];
  Vec3 p;

  Poly(const Poly &poly){
    p=poly.p;
    for(int ii=0;ii<4;ii++){
      coef[ii]=poly.p[ii];
    }
  }
};

///@brief goal is also an array of floats
class FloatGoal:public Goal{
public:
  std::vector<float> goal;
  size_t size(){return goal.size();};
  float operator[](int idx)const{return goal[idx];}
  float &operator[](int idx){return goal[idx];}
};

///@brief state represented as an array of floats
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

///@brief a gradient interface that provides
///function for computing gradient and objective value
class Gradient{
public:
  virtual void getGrad(const FloatState & state, std::vector<float>*grad)=0;
  virtual float getObjective(const FloatState & state)=0;
};

///@brief a general optimizer that uses the Gradient object
class GradOpt:public Optimizer {
public:
  FloatState state;
  Gradient * _gradient;
  int iter;
  float stepSize;
  GradOpt():iter(300),stepSize(0.01){}
  void run();
  void setState(State * _state );
  State * getState(){return new FloatState(state);}

private:
  ///makes d = a+c*b
  void addVector(const std::vector<float> &a,const std::vector<float>& b, float c,
           std::vector<float> & d)
  {
    for(size_t ii=0;ii<a.size();ii++){
      d[ii] = a[ii]+b[ii]*c;
    }
  }
};

void GradOpt::setState(State * _state)
{
  state=*(FloatState*)_state;
}

void
GradOpt::run(){
  std::vector<float>grad;
  real_t obj0 = _gradient->getObjective(state);
  for(int ii=0;ii<iter;ii++){
    _gradient->getGrad(state, &grad);
    float step = 0.1;
    FloatState srchState;
    srchState.a.resize(state.size());
    float obj;
    int nStep=0;
    do{
      addVector(state.a, grad, step, srchState.a);
      obj = _gradient->getObjective(srchState);
      step/=2;
      nStep++;
    }while(obj>=obj0 && nStep<6);
    state=srchState;
    path->push_back(Vec3(state[0],state[1],state[2]));
    std::cout<<state[0]<<" "<<state[1]<<" "<<state[2]<<" "<<state[3]<<"\n";
    std::cout<<obj<<"\n";
    obj0=obj;
  }
}

class FloatOut:public Output{
public:
  std::vector<float> a;

};

class PolyGen:public Generator{
public:
  FloatGoal goal;
  FloatState state;
  Poly poly;
  PolyGen(){
    state.a.resize(4);
    for(int ii=0;ii<4;ii++){
      state[ii]=0;
    }
  }

  void initShape(const Shape * m){
    poly=*(Poly*)m;
  }

  State * getInitialState(){
    return new FloatState(state);
  }

  void setState(const State * s){
    state=*(FloatState*)s;
  }

  Shape * getShape(){
    Poly * ret = new Poly(poly);
    for(int ii=0;ii<3;ii++){
      ret->p[ii]=state[ii];
    }
    return ret;
  }

	Output* getOutput(){
    FloatOut * out=new FloatOut();
    out->a.resize(3);
    for(int ii=0;ii<3;ii++){
      out->a[ii]=state[ii];
    }
    return out;
	}

	Gradient * getGradObj();

  class PolyGrad:public Gradient{
    public:
    explicit PolyGrad( PolyGen* _poly):polygen(_poly){}

    void getGrad(const FloatState & state, std::vector<float>*grad){
      Poly & poly = polygen->poly;
      FloatGoal & goal =polygen->goal;
      (*grad).resize(4);
      float surfDist = -poly.coef[3];
      for(int ii=0;ii<3;ii++){
        surfDist += state[ii]*state[ii]*poly.coef[ii];
      }
      for(int ii=0;ii<3;ii++){
        (*grad)[ii] = 2* (1+2*state[3]*poly.coef[ii])
                       * (state[ii]-goal[ii]+2*poly.coef[ii]*state[3]*state[ii])
                       + poly.coef[ii]*state[ii]*surfDist;
        (*grad)[ii] = -(*grad)[ii];

      }
      (*grad)[3]=0;
      for(int ii=0;ii<3;ii++){
        (*grad)[3] += 4*(state[ii]-goal[ii]+2*poly.coef[ii]*state[3]);
      }
      (*grad)[3] = -(*grad)[3];
    }

    float getObjective(const FloatState & state)
    {
      Poly & poly = polygen->poly;
      float obj=0;
      FloatGoal & goal =polygen->goal;
      float surfDist = -poly.coef[3];
      for(int ii=0;ii<3;ii++){
        surfDist += state[ii]*state[ii]*poly.coef[ii];
      }
      obj=surfDist*surfDist;
      for(int ii=0;ii<3;ii++){
        float grad = 4 * (state[ii]-goal[ii]+2*poly.coef[ii]*state[3]*state[ii]);
        obj += 4*grad*grad;
      }
      return obj;
    }
    private:
      PolyGen* polygen;
  };
};

Gradient * PolyGen::getGradObj()
{
  return new PolyGrad(this);
}


class FloatSim:public Simulator{
public:
  Poly poly;
  Scene scene;
  void setShape(Shape * shape){
    poly=*(Poly*)shape;
  }
  void setScene(Scene * _scene){
    scene=*_scene;
  }
  Goal * run(){
    FloatGoal * goal = new FloatGoal();
    goal->goal.resize(3);
    for(int ii=0;ii<3;ii++){
      goal->goal[ii]=poly.p[ii];
    }
    return goal;
  }
};

class FloatL2:public Metric{
public:
  float getError(const Goal & soln){
    float err=0;
    FloatGoal * gen=(FloatGoal*)&soln;
    for(size_t ii=0;ii<goal.size();ii++){
      float diff = goal[ii]-(*gen)[ii];
      err += diff*diff;
    }
    return err;
  }
  FloatGoal goal;
};

///@class GradManager
///@brief this manager passes the goal to the generator
///so that the generator can compute gradients.
///The manager then passes the generator's gradient and
///objective functions to
///a generic gradient descent optimizer
class GradManager:public Manager{
public:
  GradManager(){
    addMetric   ( new FloatL2());
    addOptimizer( new GradOpt());
    addSimulator( new FloatSim());
    addGenerator( new PolyGen());
  }
  void addGoal(Goal * _goal);
  void setShape(Shape * shape);
  void run();
  Output*getOutput();
};

void GradManager::addGoal(Goal * _goal)
{
  ((FloatL2*)(metric[0]))    -> goal=*(FloatGoal*)_goal;
  ((PolyGen*)(generator[0])) -> goal=*(FloatGoal*)_goal;
}

void GradManager::setShape(Shape * m)
{
  ((PolyGen*)(generator[0])) -> initShape(m);
}

void GradManager::run(){
  State * state;
  state = generator[0] -> getInitialState();
  Gradient * gradient;
  gradient=((PolyGen*)(generator[0])) -> getGradObj();
  optimizer[0]-> setState(state);
  ((GradOpt*)(optimizer[0])) ->_gradient = gradient;
  optimizer[0]->run();
  delete state;
  state = optimizer[0]->getState();
  generator[0]->setState(state);
}

Output* GradManager::getOutput()
{
  return generator[0]->getOutput();
}

/////////////Below is input API

int main1(std::vector<Vec3> * points){
  path = points;

  Poly p;
  for(int ii=0;ii<4;ii++){
    p.coef[ii]=1+ii;
  }
  p.coef[0]=-1;
  FloatGoal goal;
  goal.goal.resize(3);
  for(int ii=0;ii<3;ii++){
    goal[ii]=0;
  }
  goal[1]=2;
  goal[0]=1;
  goal[2]=1;
  GradManager manager;
  manager.setShape(&p);
  manager.addGoal(&goal);
  manager.run();
  FloatOut * out = (FloatOut*)manager.getOutput();

  for(int ii=0;ii<3;ii++){
    std::cout<<out->a[ii]<<" ";
  }
  std::cout<<"\n";
  return 0;
}
