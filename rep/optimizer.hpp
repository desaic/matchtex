#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <vector>
#include <math.h>
#include <stdlib.h>
#include <cstdlib>

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
    virtual State * getState()=0;
    virtual void setState(State * state)=0;
    virtual void run()=0;
	};

}
#endif
