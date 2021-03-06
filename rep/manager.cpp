#include "manager.hpp"
#include <iostream>
namespace Fab{
Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::addSimulator( Simulator * s) {
  simulator.push_back(s);
}

void Manager::addMetric( Metric * s) {
  metric.push_back(s);
}

void Manager::addOptimizer( Optimizer * s){
  optimizer.push_back(s);
}

void Manager::addGenerator( Generator * s){
  generator.push_back(s);
}

void Manager::setShape(Shape * m)
{
  shape=m;
}

void Manager::addScene(Scene * _scene)
{
  scene.push_back(_scene);
}

void Manager::addGoal( Goal* _goal)
{
  goal.push_back(_goal);

}

void Manager::SetLogLevel(int level) {
  // To be implemented
}
void Manager::run(){
  std::cout<<"Unimplemented\n";
}
Output* Manager::getOutput(){
  std::cout<<"Unimplemented\n";
  return 0;
}

}
