#ifndef GENERATOR_HPP
#define GENERATOR_HPP

namespace Fab{
  class Generator {
	public:
		virtual void initShape( const Shape * m )=0;
	  virtual void setState(const State * s)=0;
    virtual Shape  * getShape()=0;
		virtual Output * getOutput()=0;
		virtual  State * getInitialState() = 0 ;
	};
}

#endif
