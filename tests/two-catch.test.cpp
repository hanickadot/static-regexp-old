#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRX;

using TwoCatch = Star<Chr<' '>,DynamicCatch<10,Plus<Chr<'a','b','c'>>, Star<Chr<'X'>>>, Chr<'='>, DynamicCatch<20,Plus<Chr<'i','j','k'>>>>;

// input ' ab=ij a=i abab=j b=jiji'
// paths:
// Star< >, End, Closure
//  

bool sameCount(const char * str, unsigned int needCount)
{
	RegularExpression<TwoCatch> regexp;
	//printf("regexp: '%s'\n", str);
	if (regexp(str))
	{
		unsigned int a = regexp.getCatch<10>().size();
		unsigned int b = regexp.getCatch<20>().size();
		//printf(" a = %u, b = %u\n", a, b);
		if ((a != b) || (needCount != a))
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
		unsigned int id{0};
		for (auto tmp: regexp.getCatch<10>()) fprintf(stderr,"10.%u: '%.*s' %u..%u\n",id++,(int)tmp.length,str+tmp.begin, tmp.begin,tmp.length);
		id = 0;
		for (auto tmp: regexp.getCatch<20>()) fprintf(stderr,"20.%u: '%.*s' %u..%u\n",id++,(int)tmp.length,str+tmp.begin, tmp.begin,tmp.length);
		id = 0;
		return true;
	}
	fprintf(stderr,"Regexp didn't match '%s'\n",str);
	return false;
}

int main ()
{
	if (!sameCount(" ab=ij a=i abab=j b=jiji",4)) return 1;
	if (!sameCount(" aX=ik aXXXX=i aXXXX=ijij",3)) return 1;
	if (!sameCount(" ad=ij",1)) return 1;
	return 0;
}
