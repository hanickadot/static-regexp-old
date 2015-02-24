#include <regexp/regexp.hpp>

using namespace SRX;

using Alpha = CRange<'a','z'>;
using AlphaNum = CRange<'a','z','0','9'>;
using TwoCycles = Plus<Seq<Chr<' '>,DynamicCatch<0,Plus<Alpha>>,DynamicCatch<1,Plus<AlphaNum>>,Chr<'='>,DynamicCatch<2,Plus<Alpha>>>>;

std::string getStringFromCatch(const char * str, Catch ctch)
{
	return std::string(str+ctch.begin, ctch.length);
}

template <unsigned int id, typename Regexp> bool checkExpected(const char * str, const Regexp regexp, const std::vector<const std::string> & expected)
{
	if (expected.size() == regexp.template getCatch<id>().size())
	{
		for (unsigned int i{0}; i!=expected.size(); ++i)
		{
			//if (i <= regexp.template getCatch<id>().size())
			std::string get{getStringFromCatch(str,regexp.template getCatch<id>()[i])};
			std::string ex{expected[i]};
			if (get != ex)
			{
				fprintf(stderr,"testOne('%s') failed (expected string diff get:'%s' vs ex:'%s')\n",str,get.c_str(),ex.c_str());
				exit(1);
				return false;
			}
		}
		return true;
	}
	else
	{
		fprintf(stderr,"testOne('%s') failed (expected[%u] size diff get:%zu vs ex:%zu).\n",str,id, regexp.template getCatch<id>().size(),expected.size());
		exit(1);
		return false;
	}
}

template <typename RegexpType = TwoCycles> bool testOne(const char * str, std::vector<std::vector<const std::string>> && expected)
{
	printf("\n\ntest for '%s'\n",str);
	RegularExpression<Begin,RegexpType,End> regexp;
	if (regexp.match(str))
	{
		unsigned int id{0};
		
		for (auto tmp: regexp.template getCatch<0>()) printf("0.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
		id = 0;
		for (auto tmp: regexp.template getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
		id = 0;
		for (auto tmp: regexp.template getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,str+tmp.begin);
		id = 0;
		
		checkExpected<0>(str, regexp, expected[0]);
		checkExpected<1>(str, regexp, expected[1]);
		checkExpected<2>(str, regexp, expected[2]);
		
		return true;
	}
	fprintf(stderr,"testOne('%s') failed.\n",str);
	exit(1);
	return false;
}

using TwoCycles2 = Plus<Seq<Chr<' '>,DynamicCatch<0,Plus<Alpha>>,Plus<AlphaNum>,Chr<'='>,DynamicCatch<1,Alpha>>>;

int main()
{
	RegularExpression<Begin,TwoCycles,End> regexp;
	
	testOne(" abcabc9a=abc", {{"abcabc"},{"9a"},{"abc"}});
	testOne(" a9a=a", {{"a"},{"9a"},{"a"}});
	testOne(" a1a=a b2b=b", {{"a","b"},{"1a","2b"},{"a","b"}});
	testOne(" a1a=a b2b=b c3c=c", {{"a","b","c"},{"1a","2b","3c"},{"a","b","c"}});
	testOne<TwoCycles2>(" a1a=a ccc=b", {{"a","cc"},{"a","b"}});
	testOne(" a1a=a ccc=b", {{"a","cc"},{"1a","c"},{"a","b"}});
	
	return 0;
}
