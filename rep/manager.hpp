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
  void addSimulator( Simulator * s);
  void addMetric( Metric * s);
  void addOptimizer( Optimizer * s);
  void addGenerator( Generator * s);
  void setShape(Shape & m);
  void addScene(Scene * _scene);
  void addGoal( Goal* _goal);
  virtual void run();
  virtual Output getOutput();
  void SetGoal( Goal * goal);
  void SetLogLevel(int level);
  Params * GetResults();
 private:
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
