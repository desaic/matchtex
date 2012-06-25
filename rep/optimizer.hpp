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

class Params {
public:
	virtual int nDim() = 0;
	virtual void Copy(Params * params) = 0;
};
class Constraint {
public:
	// Constraint
	class FunctionConstraint{
	};

	enum ConstraintType{
		CONSTRAINT_LINEAR,
		CONSTRAINT_FUNCTION,
	} ;
	// Linear Constraints
//	Constraint();std::vector< float> coefficients);
//	Constraint(FunctionConstraint func);
};



// Domain
//    - Set + Constraints
//    - SearchTree/Graph

template < class Path >
class PartialSolutionResolver{
	virtual void getBoundSolution(Path * currentPath, Path **minPath, Path **maxPath) {};
	virtual bool isBoundableSolution() { return false; }
	virtual void getBoundSolution(Path * currentPath, float &minVal) { };
	virtual bool isBoundable() { return false; }

	virtual void intermediateResolve(Path *currentPath, vector<float> &result) {};
	virtual bool isIntermediateResolvable() { return false; }
	virtual int  nDim() { return 0; }
};


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
  class SearchSpace{
  };
	struct Bound {
		double mmin, mmax;
	};

	class Simulator {
	public:
	  virtual void setShape(Shape * Shape) = 0;
		virtual void setScene(Scene *scene) = 0;
	  virtual Goal * run(){
	    return 0;
	  };
	};

	class Metric{
	public:
		virtual float getError( const Goal & constructed) = 0;
		Goal goal;
	};

	class Generator {
	public:
		//typedef typename SearchSpace::State              State;
		virtual void initShape( const Shape * m )=0;
		//is this necessary?
		//virtual void initScene(Scene scene);
    virtual void setState(const State * s)=0;
    virtual Shape * getShape()=0;
		virtual Output * getOutput()=0;
		virtual  State  *          getInitialState() = 0 ;
	//	virtual  SearchSpace       getSearchSpace () = 0 ;
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
