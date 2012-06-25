#ifndef METRIC_HPP
#define METRIC_HPP
namespace Fab{
  class Metric{
	public:
		virtual float getError( const Goal & constructed) = 0;
		Goal goal;
	};
}
#endif
