#include "regexp/regexp.hpp"

using namespace SRX;

int main()
{
	RegularExpression<Begin, Opt<DynamicCatch<1,Plus<Selection<String<'a','b'>,String<'c','d'>>>>>, Chr<'x','y','z'>, CRange<'0','9'>, End> regexp;
	
	std::cout << regexp << "\n";
	return 0;
}