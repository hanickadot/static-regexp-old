#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

using TwoCatch = Star<Chr<' '>,DynamicCatch<10,Plus<Chr<'a','b','c'>>, Star<Chr<'X'>>>, Chr<'='>, DynamicCatch<20,Plus<Chr<'a','b','c'>>>>;

bool sameCount(const char * str)
{
	RegularExpression<Begin,TwoCatch,End> regexp;
	if (regexp(str))
	{
		unsigned int a = regexp.getCatch<10>().size();
		unsigned int b = regexp.getCatch<20>().size();
		
		if (a != b)
		{
			fprintf(stderr,"Catch for A and B doesn't have same count! (%u vs %u)\n",a,b);
			fprintf(stderr,"input = '%s'\n",str);
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<10>()) fprintf(stderr,"10.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<20>()) fprintf(stderr,"20.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
			return false;
		}
		return true;
	}
	fprintf(stderr,"Regexp didn't match '%s'\n",str);
	return false;
}

int main ()
{
	if (!sameCount(" ab=ab a=a abab=b b=baba")) return 1;
	if (!sameCount(" aX=ac aXXXX=a aXXXX=abab")) return 1;
	if (!sameCount(" ad=ab")) return 1;
	return 0;
}
