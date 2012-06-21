#include "searchtree.h"
#include "optimizer.h"

#include <iostream>
#include <tuple>

#include <boost/units/systems/si/energy.hpp>
#include <boost/units/systems/si/force.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/electric_potential.hpp>
#include <boost/units/systems/si/current.hpp>
#include <boost/units/systems/si/resistance.hpp>
#include <boost/units/systems/si/io.hpp>
#include <complex>
#include <iostream>

#include <boost/mpl/list.hpp>

#include <boost/typeof/std/complex.hpp>

#include <boost/units/pow.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/io.hpp>

int x (int f) {
	return f+2.1;
}

// Homo Subsurface

namespace HomogeneousBSSRDF {
	using namespace FabricationAbstraction;

	class DeviceIndependentLanguage {
	public:
//		std::vector< double >   m_Scurve;
//		std::vector< double >   m_Scurve;
	};

	class DeviceDependentLaguage {
	public:
	};

	struct MaterialStack {
		std::vector< double >   m_Scurve;
		std::vector< double >   m_Tcurve;
		int    m_newstacking;
		double m_thickness;
//		PyObject *pyobject;
	};

	class StackSearch {
		class State {};


	};

	class HomogeneousBSSRDFReducer :
		public Reducer< DeviceIndependentLanguage, DeviceDependentLaguage, StackSearch> {

		void setProblem( DeviceIndependentLanguage goal,
			Simulator< DeviceIndependentLanguage, DeviceDependentLaguage> * simulator,
			Metric   < DeviceIndependentLanguage, DeviceDependentLaguage>   * metric) {


		};
	//	PartialFunction getPartialErrorFunction() { return NULL; };
		ErrorFunction   getErrorFunction() { return NULL; };
	//	TestFunction     getTestFunction() { return NULL; };
		State            getInitialState() { return State(); };
	//	StackSearch          getSearchSpace ()   { return   StackSearch();   } ;
		DeviceDependentLaguage  toDeviceLanguage(State )  { return DeviceDependentLaguage();  };

	};

};




int main () {
	// Tester
	{
		GenericPath<int > genericpath;
		EmptyTree  t;
	}
	typedef GenericPath<int> GP;
	BFS< GP , EmptyTree> opt;
	auto i = 3;

	std::tuple< int, int, float > a(1,2,3);
	std::function< int (int) > P = &x;
	std::cout<< std::get<1>(a) << ", " << P(1) << endl;


	using namespace boost::units;
	using namespace boost::units::si;
    quantity<length> L = 2.0*meters;                     // quantity of length
    quantity<energy> E = kilograms*pow<2>(L/seconds);    // quantity of energy
    auto E2 = pow<2>(L/seconds);    // quantity of energy
	auto E3 = (L/seconds);    // quantity of energy

	auto E4 = L/ L;

	std::cout << E << ", " << E2 << endl;
	std::cout << E3 << endl;
	std::cout << E4 << endl;

//	StackOptimizer<GenericPath> opt;
	return 0;
}
