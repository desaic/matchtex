#pragma once

#ifndef __OPTIMIZER__
#define __OPTIMIZER__

#include <vector>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>
//#include "set.h"
//#include "searchtree.h"

#include <functional>
//#include <boost/icl/interval_set.hpp>

#define debug(S, ...) fprintf(stderr, S, __VA_ARGS__);

using namespace std;

namespace Fab {

  class Goal{
  };
  class Scene{
  };
  class Shape{
  };
  class Output{
  };
  class State{
  };

  class Optimizer {
	public:
//		typedef typename SearchSpace::State              State;
	//	virtual void setSearchSpace( SearchSpace searchSpace) = 0;

    virtual State * getState()=0;
    virtual void setState(State * state)=0;
		//virtual void run( int step) = 0;
		//virtual void run( double errorThreshold ) = 0;
    virtual void run()=0;
		//virtual void runStep() = 0;
	};

}
#endif
