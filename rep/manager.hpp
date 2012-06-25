#ifndef __MANAGER__
#define __MANAGER__

#include "optimizer.hpp"
//#include "libobject.h"
#include <vector>
using namespace std;
namespace Fab {
/**@brief
*/
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
  Params * m_result;
};
}
#endif
