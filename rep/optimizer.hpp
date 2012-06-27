#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "fabStruct.hpp"

#include <vector>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;

namespace Fab {

  ///@brief the optimizer finds the optimal state in the state space
  class Optimizer {
	public:
    virtual State * getState()=0;
    virtual void setState(State * state)=0;
    virtual void run()=0;
	};

}
#endif
