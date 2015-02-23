#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRX;

using CatchInCatch = OneCatch<1, Str<'a','b','c'>, OneCatch<2, Chr<'b'>>>;

std::string getStringFromCatch(const char * str, Catch ctch)
{
	return std::string(str+ctch.begin, ctch.length);
}

bool catchInCatch()
{
	RegularExpression< CatchInCatch > regexp;
	const char * str = "abcb";
	if (regexp(str))
	{
		//unsigned int count{0};
		//printf("FIRST = %zu\n", regexp.getCatch<1>().size());
		//printf("SECOND = %zu\n", regexp.getCatch<2>().size());
		//for (auto tmp: regexp.getCatch<1>()) printf("FIRST: '%.*s'\n",(int)tmp.length,str+tmp.begin);
		//for (auto tmp: regexp.getCatch<2>()) printf("SECOND: '%.*s'\n",(int)tmp.length,str+tmp.begin);
		//return true;
		
		return regexp.getCatch<1>().size() == 1 && getStringFromCatch(str,regexp.getCatch<1>()[0]) == "abcb" && regexp.getCatch<2>().size() == 1  && getStringFromCatch(str,regexp.getCatch<2>()[0]) == "b";
	}
	return false;
}

int main ()
{
	if (catchInCatch()) return 0;
	return 1;
}
