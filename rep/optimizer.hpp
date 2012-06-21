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
  class Mesh{
  };
  class Output{
  };
  class SearchSpace{

   public:
    class State{
    };
  };
	struct Bound {
		double mmin, mmax;
	};

	class Simulator {
	public:
		virtual Goal simulate(Scene scene) = 0;
	};

	class Metric{
	public:
		virtual double compare( Goal constructed, Goal target) = 0;
	};

	class Generator {
	public:
		typedef typename SearchSpace::State              State;
		virtual void initMesh( Mesh m );
		//is this necessary?
		//virtual void initScene(Scene scene);
    virtual void setState(State s);
    virtual Mesh getMesh();
		virtual Output getOutput();
		virtual  State            getInitialState() = 0 ;
		virtual  SearchSpace       getSearchSpace () = 0 ;
	};

	class Optimizer {
	public:
		typedef typename SearchSpace::State              State;
		virtual void setSearchSpace( SearchSpace searchSpace) = 0;

    virtual State getState()=0;
		virtual void run( int step) = 0;
		virtual void run( double errorThreshold ) = 0;
		virtual void runStep() = 0;
	};

}
#endif
