#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP
namespace Fab{
class Simulator {
	public:
	  virtual void setShape(Shape * Shape) = 0;
		virtual void setScene(Scene *scene) = 0;
	  virtual Goal * run(){
	    return 0;
	  };
	};
}
#endif
