#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main (int argc, char const *argv[])
{
	RegularExpression<Begin,OneCatch<1,Plus<CRange<'a','z'>>>,End> regexp;
	
	if (argc >= 2)
	{
		if (regexp(argv[1]))
		{
			printf("yes:\n");
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<0>()) printf("0.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<3>()) printf("3.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<4>()) printf("4.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
