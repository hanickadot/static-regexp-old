#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

using TwoCatch = Star<Chr<' '>,DynamicCatch<10,Plus<Chr<'a','b','c'>>, Star<Chr<'X'>>>, Chr<'='>, DynamicCatch<20,Plus<Chr<'i','j','k'>>>>;

// input ' ab=ij a=i abab=j b=jiji'
// paths:
// Star< >, End, Closure
//  

bool sameCount(const char * str)
{
	RegularExpression2<TwoCatch, End> regexp;
	if (regexp(str))
	{
		unsigned int a = regexp.getCatch<10>().size();
		unsigned int b = regexp.getCatch<20>().size();
		
		if (a != b)
		{
			fprintf(stderr,"Catch for A and B doesn't have same count! (%u vs %u)\n",a,b);
			fprintf(stderr,"input = '%s'\n",str);
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<10>()) fprintf(stderr,"10.%u: '%.*s' %u..%u\n",id++,(int)tmp.length,str+tmp.begin, tmp.begin,tmp.length);
			id = 0;
			for (auto tmp: regexp.getCatch<20>()) fprintf(stderr,"20.%u: '%.*s' %u..%u\n",id++,(int)tmp.length,str+tmp.begin, tmp.begin,tmp.length);
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
	if (!sameCount(" ab=ij a=i abab=j b=jiji")) return 1;
	if (!sameCount(" aX=ik aXXXX=i aXXXX=ijij")) return 1;
	if (!sameCount(" ad=ij")) return 1;
	return 0;
}
