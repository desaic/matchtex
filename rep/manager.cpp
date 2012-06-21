#include "manager.hpp"

namespace Fab{
Manager::Manager() {
  m_result = NULL;
}

Manager::~Manager() {
  if (m_result)
    delete m_result;
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

void Manager::addScene(Scene * _scene)
{

}

void Manager::addGoal( Goal* _goal)
{

}

void Manager::SetLogLevel(int level) {
  // To be implemented
}
}