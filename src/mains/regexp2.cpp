#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main (int argc, char const *argv[])
{
	//using regexp = RegularExpression<Begin,Chr<'a'>,Selection<Chr<'b'>,Chr<'c'>,Empty>,End>;
	
	using ahoj = String<'a','h','o','j'>;
	//////
	////template <typename... Inner> using Plus = Sequence<Inner..., Star<Inner...>>;
	using mojeplus = Plus<Chr<'y'>,Chr<'z'>>;
	using mojeseq = Seq<Begin, Sel< Chr<'x'>, mojeplus>, Star<Chr<'a'>>,End>;
	using velkyvyber = Sel<mojeseq, ahoj >;
	using regexp = RegularExpression<velkyvyber>;
	
	//using regexp = RegularExpression<Begin,Optional<ahoj>,End>;
	
	//using regexp = RegularExpression<Begin,Optional<Chr<'x'>>,End>;
	//using regexp = RegularExpression<Begin,Seq<Seq<Seq< Plus<Chr<'a'>> >>>, Star<Chr<'b'>>, End>;
	if (argc >= 2)
	{
		if (regexp::match(argv[1]))
		{
			printf("yes\n");
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
