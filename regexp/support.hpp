#ifndef __REGEXP__SUPPORT__HPP__
#define __REGEXP__SUPPORT__HPP__

struct RepeatCounter
{
	static unsigned int getID()
	{
		static unsigned int id = 0;
		return ++id;
	}
	static unsigned int & getDeepRef()
	{
		static unsigned int deep = 0;
		return deep;
	}
	static unsigned int getDeep()
	{
		return getDeepRef();
	}
	static void enter()
	{
		getDeepRef()++;
	}
	static void leave()
	{
		getDeepRef()--;
	}
	static std::ostream & prefix(std::ostream & stream, const char * str = ".  ")
	{
		for (unsigned int i{0}; i != getDeepRef(); ++i) stream << str;
		return stream;
	}
};

#endif
