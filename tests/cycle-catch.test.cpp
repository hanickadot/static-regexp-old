#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

using CycleCatch = Seq<Begin,DynamicCatch<1, Star<Chr<'a'>>>, DynamicCatch<2, Star<Chr<'b'>>>>;

bool justOneForEach(const char * str)
{
	RegularExpression< CycleCatch > regexp;
	if (regexp(str))
	{
		unsigned int count{0};
		for (auto tmp: regexp.getCatch<1>()) count++;
		for (auto tmp: regexp.getCatch<2>()) count++;
		if (count != 2)
		{
			fprintf(stderr,"More catches then 2 expected! (%u)\n",count);
			fprintf(stderr,"input = '%s'\n",str);
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
		}
		return count == 2;
	}
	return false;
}

int main ()
{
	if (!justOneForEach("ab")) return 1;
	if (!justOneForEach("aaa")) return 1;
	if (!justOneForEach("aaaabbbb")) return 1;
	if (!justOneForEach("aabb")) return 1;
	if (!justOneForEach("bb")) return 1;
	return 0;
}
