#ifndef __REGEXP__REGEXP2__HPP__
#define __REGEXP__REGEXP2__HPP__

#include <string>
#include <cstddef>
#include <cstdio>

//#define DEBUG	
	
#ifdef DEBUG
#define DEBUG_PRINTF(text, ...) { printTab(deep); printf("%02zu:" text,string.getPosition(), __VA_ARGS__); }
#define DEBUG_PRINT(text) { printTab(deep); printf("%02zu:" text,string.getPosition()); }
#else
#define DEBUG_PRINTF(...)
#define DEBUG_PRINT(text)
#endif

void printTab(unsigned int deep)
{
	while (deep > 0) { printf("  "); deep--; }
}

namespace SRegExp2 {

	#include "abstraction.hpp"
	
	// available templates for users:
	struct Begin; // ^
	struct End; // $
	
	template <bool positive, wchar_t... codes>  struct CharacterClass; // positive: [abc] or negative: [^abc]
	template <wchar_t... codes> struct String;
	using Empty = String<>; // placeholder to options
	template <typename... Parts> struct Sequence; // (abc)
	template <typename... Options> struct Selection; // (a|b|c)
	template <unsigned int min, unsigned int max, typename... Inner> struct Repeat; // a{min;max}
	template <typename... Inner> using Plus = Repeat<1,0,Inner...>; // (abc)+
	template <typename... Inner> using Star = Repeat<0,0,Inner...>; // (abc)*
	template <typename... Inner> using Optional = Selection<Sequence<Inner...>,Empty>; // a?
	template <typename... Definition> struct RegularExpression; 
	
	// aliases:
	using Any = CharacterClass<true>; // .
	template <wchar_t... codes> using Set = CharacterClass<true, codes...>;
	template <wchar_t... codes> using NegativeSet = CharacterClass<false, codes...>;
	template <wchar_t... codes> using NegSet = NegativeSet<codes...>;
	template <wchar_t... codes> using Character = CharacterClass<true, codes...>;
	template <wchar_t... codes> using Chr = CharacterClass<true, codes...>;
	template <typename... Parts> using Seq = Sequence<Parts...>;
	template <typename... Options> using Sel = Selection<Options...>;
	template <typename... Inner> using Opt = Optional<Inner...>;
	
	// implementation:
	
	struct Closure
	{
		template <typename StringAbstraction, typename Root, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root &, Right...)
		{
			return true;
		}
	};
	
	struct Begin
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			if (string.isBegin())
			{
				if (nright.match(string, move, deep, root, right...))
				{
					return true;
				}
			}
			return false;
		}
	};
	
	struct End
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			if (string.isEnd())
			{
				if (nright.match(string, move, deep, root, right...))
				{
					return true;
				}
			}
			return false;
		}
	};
	
	template <wchar_t firstCode, wchar_t... codes> struct String<firstCode, codes...>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			size_t pos{0};
			if (checkString(string, pos, deep))
			{
				DEBUG_PRINTF("checkString match (pos = %zu)\n",pos);
				size_t pos2{0};
				if (nright.match(string.add(pos), pos2, deep, root, right...))
				{
					move = pos+pos2;
					return true;
				}
				DEBUG_PRINT("nright not match.\n");
			}
			else
			{
				DEBUG_PRINTF("String: Character '%c' is different to requested character (first: '%c') -> NOT match\n", *string.str, firstCode);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkString(const StringAbstraction string, size_t & move, unsigned int deep)
		{
			size_t pos{0};
			if (string.equal(firstCode))
			{
				DEBUG_PRINTF("Character '%c' compared with '%c' -> match\n", *string.str, firstCode);
				if (String<codes...>::checkString(string.add(1), pos, deep))
				{
					move = pos+1;
					return true;
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' compared with '%c' -> NOT match\n", *string.str, firstCode);
			}
			return false;
		}
	};
	
	template <> struct String<>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			return nright.match(string, move, deep, root, right...);
		}
		template <typename StringAbstraction> static inline bool checkString(const StringAbstraction string, size_t & move, unsigned int deep)
		{
			return true;
		}
	};
	
	template <bool positive, wchar_t firstCode, wchar_t... code> struct CharacterClass<positive, firstCode, code...>
	{
		static const constexpr bool isEmpty{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character (first: '%c') -> NOT match\n", *string.str, firstCode);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int deep)
		{
			if (string.equal(firstCode)) 
			{
				DEBUG_PRINTF("Character compared with '%c' -> match\n", firstCode);
				return true;
			}
			else if (!Character<code...>::isEmpty && Character<code...>::checkChar(string, deep)) return true;
			else return false;
		}
	};
	
	template <bool positive> struct CharacterClass<positive>
	{
		static const constexpr bool isEmpty{true};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character -> NOT match\n", *string.str);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int deep)
		{
			if (string.exists()) 
			{
				DEBUG_PRINT("Character compared with ANY -> match\n");
				return true;
			}
			return false;
		}
	};
	
	template <typename FirstOption, typename... Options> struct Selection<FirstOption, Options...>
	{
		FirstOption first;
		Selection<Options...> restopt;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			if (first.match(string, move, deep+1, root, nright, right...))
			{
				return true;
			}
			else
			{
				return restopt.match(string, move, deep+1, root, nright, right...);
			}
		}
	};
	
	template <> struct Selection<>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			return false;
		}
	};
	
	template <typename First, typename... Rest> struct Sequence<First, Rest...>
	{
		First first;
		Sequence<Rest...> restseq;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			if (first.match(string, move, deep, root, restseq, nright, right...))
			{
				return true;
			}
			return false;
		}
	};

	template <typename First> struct Sequence<First>
	{
		First first;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			return first.match(string, move, deep, root, nright, right...);
		}
	};
	
	template <unsigned int min, unsigned int max, typename Inner, typename... Rest> struct Repeat<min, max, Inner, Rest...>
	{
		Repeat<min, max, Seq<Inner,Rest...>> innerRepat;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			return innerRepat.match(string, move, deep, root, nright, right...);
		}
	};
	
	template <unsigned int min, unsigned int max, typename Inner> struct Repeat<min, max, Inner>
	{
		Inner inner;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			size_t pos{0};
			ssize_t lastFound{-1};
			Closure closure;
			for (size_t cycle{0}; (!max) || (cycle < max); ++cycle)
			{
				size_t tmp{0};
				if (nright.match(string.add(pos), tmp, deep+1, root, right...) && (cycle >= min))
				{
					lastFound = pos + tmp;
					DEBUG_PRINTF(">> found at %zu\n",lastFound);
					pos += tmp;
				}
				// in next expression "empty" is needed
				if (inner.match(string.add(pos), tmp, deep+1, root, closure))
				{
					pos += tmp;
				}
				else if (lastFound >= 0)
				{
					DEBUG_PRINTF("cycle done (cycle = %zu)\n",cycle);
					move += static_cast<size_t>(lastFound);
					return true;
				}
				else break;
			}
			return false;
		}
	};
	
	template <typename... Inner> struct Eat
	{
		Sequence<Inner...> inner;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, NearestRight & nright, Right... right)
		{
			size_t pos{0};
			
			if (!string.exists() && inner.match(string, move, deep+1, root, nright, right...))
			{
				// branch just for empty strings
				return true;
			}
			else while (string.exists(pos)) {
				size_t imove{0};
				//DEBUG_PRINTF("eating... (pos = %zu)\n",pos);
				if (inner.match(string.add(pos), imove, deep+1, root, nright, right...))
				{
					move += pos + imove;
					return true;
				}
				else pos++;
			}
			return false;
		}
	};
	
	template <typename... Definition> struct RegularExpression
	{
		Eat<Definition...> eat;
		bool operator()(std::string string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const char *>(string.c_str()), pos, 0, eat, closure);
		}
		bool operator()(const char * string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const char *>(string), pos, 0, eat, closure);
		}
		template <typename StringType> static constexpr bool match(StringType string)
		{
			RegularExpression<Definition...> regexp{};
			return regexp(string);
		}
	};
}

#endif