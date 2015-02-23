#include "regexp/regexp.hpp"

using namespace SRX;

int main()
{
	RegularExpression<Begin, DynamicCatch<1,Plus<String<'a','b'>,String<'a','b'>>>, End> regexp;
	
	//std::cout << regexp << "\n";
	return 0;
}