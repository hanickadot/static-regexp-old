#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

using CycleCatch = Seq<Begin,DynamicCatch<1, Star<Chr<'a'>>>, DynamicCatch<2, Star<Chr<'b'>>>>;

bool justOneForEach(const char * str)
{
	printf("\n\n\n-----\n");
	RegularExpression< CycleCatch > regexp;
	if (regexp(str))
	{
		unsigned int count{0};
		count += regexp.getCatch<1>().size();
		count += regexp.getCatch<2>().size();
		
		if (count != 2)
		{
			fprintf(stderr,"More catches then 2 expected! (%u)\n",count);
			fprintf(stderr,"input = '%s' (%u)\n",str,count);
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
			id = 0;
			return false;
		}
		unsigned int id{0};
		fprintf(stderr,"input = '%s' (%u)\n",str,count);
		for (auto tmp: regexp.getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
		id = 0;
		for (auto tmp: regexp.getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
		id = 0;
		return true;
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
