#include "regexp/experiment.hpp"
#include <cstdio>

//using namespace SRegExp;

int main (int argc, char const *argv[])
{
	using moje = Experiment<Check<'a'>,Sequence<Check<'b'>, Sequence< Sequence<Check<'c'>> , Check<'d'> > >, Check<'e'> >;
	
	moje::call();
	
	printf("----\n");
	
	using bc_t = Sequence<Check<'b'>, Check<'c'>>;
	using moje2 = Experiment<Check<'a'>, bc_t, Check<'d'>, Check<'e'>>;
	moje2::call();
	
	return 0;
}
