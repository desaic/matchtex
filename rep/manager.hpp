#ifndef __MANAGER__
#define __MANAGER__

#include "optimizer.hpp"
#include "generator.hpp"
#include <vector>
using namespace std;
namespace Fab {
///@class Manager
///@brief manages information required by different components of the
///optimization procedure
class Manager {
public:
  Manager();
  ~Manager();
  virtual void addSimulator( Simulator * s);
  virtual void addMetric( Metric * s);
  virtual void addOptimizer( Optimizer * s);
  virtual void addGenerator( Generator * s);
  virtual void setShape(Shape * m);
  virtual void addScene(Scene * _scene);
  virtual void addGoal( Goal* _goal);
  virtual void run();
  virtual Output* getOutput();
  void SetLogLevel(int level);
 //Params * GetResults();
protected:
  std::vector<Scene*>   scene;
  std::vector<Goal *>   goal;
  Shape * shape;
  vector< Simulator* >  simulator;
  vector< Metric * >    metric;
  vector< Generator * >   generator;
  vector< Optimizer *>  optimizer;
};
}
#endif
