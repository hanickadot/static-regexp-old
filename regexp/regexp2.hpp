#ifndef __REGEXP__REGEXP2__HPP__
#define __REGEXP__REGEXP2__HPP__

#include <string>
#include <vector>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>

//#define DEBUG	
	
#ifdef DEBUG
#define DEBUG_PRINTF(text, ...) { printf("%02zu:" text,string.getPosition(), __VA_ARGS__); }
#define DEBUG_PRINT(text) { printf("%02zu:" text,string.getPosition()); }
#else
#define DEBUG_PRINTF(...)
#define DEBUG_PRINT(text)
#endif

#pragma GCC diagnostic ignored "-Wtype-limits"

//void printTab(unsigned int deep)
//{
//	while (deep > 0) { printf("  "); deep--; }
//}

#include "abstraction.hpp"

namespace SRegExp2 {
	
	// available templates for users:
	struct Begin; // ^
	struct End; // $
	
	template <bool positive, wchar_t... codes>  struct CharacterClass; // positive: [abc] or negative: [^abc]
	template <bool positive, wchar_t... rest> struct CharacterRange; // positive [a-b] or negative [^a-b]
	template <wchar_t... codes> struct String;
	using Empty = String<>; // placeholder to options
	template <typename... Parts> struct Sequence; // (abc)
	template <typename... Options> struct Selection; // (a|b|c)
	template <unsigned int min, unsigned int max, typename... Inner> struct Repeat; // a{min;max}
	template <typename... Inner> using Plus = Repeat<1,0,Inner...>; // (abc)+
	template <typename... Inner> using Star = Repeat<0,0,Inner...>; // (abc)*
	template <typename... Inner> using Optional = Selection<Sequence<Inner...>,Empty>; // a?
	template <unsigned int id, typename MemoryType, typename... Inner> struct CatchContent; // catching content of (...)
	template <unsigned int baseid, unsigned int catchid = 0> struct ReCatch; // ([a-z]) \1
	template <unsigned int baseid, unsigned int catchid = 0> struct ReCatchReverse; // ([a-z]) \1
	template <typename... Definition> struct RegularExpression; 
	
	// MemoryTypes for CatchContent
	template <size_t size> struct StaticMemory;
	struct DynamicMemory;
	using OneMemory = StaticMemory<1>;
	
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
	template <unsigned int id, typename... Inner> using OneCatch = CatchContent<id, OneMemory, Inner...>;
	template <unsigned int id, size_t size, typename... Inner> using StaticCatch = CatchContent<id, StaticMemory<size>, Inner...>;
	template <unsigned int id, typename... Inner> using DynamicCatch = CatchContent<id, DynamicMemory, Inner...>;
	template <wchar_t a, wchar_t b, wchar_t... rest> using CRange = CharacterRange<true, a, b, rest...>;
	template <wchar_t... codes> using Str = String<codes...>;
	using Space = Chr<' '>;
	using WhiteSpace = Set<' ','\t','\r','\n'>;
	
	template <typename CharType> using CompareFnc = bool (*)(const CharType, const CharType, const CharType);
	
	// implementation:
	template <typename T> struct Reference
	{
		T & target;
		inline Reference(T & ltarget): target(ltarget) { }
		inline T & getRef() { return target; }
	};
	
	struct CatchReturn;
	
	// last item in recursively called regexp which always return true for match call
	// it must be always used as last item of call-chain
	struct Closure
	{
		template <typename StringAbstraction, typename Root, typename... Right> inline bool match(const StringAbstraction, size_t &, unsigned int, Root &, Right...)
		{
			return true;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight>, Right...)
		{
			
		}
		inline void reset()
		{
			
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// Memory for all right context
	
	template <typename T> struct CheckMemory
	{
		static const constexpr bool have = false;
	};
	
	template <typename... Rest> struct AllRightContext;
	
	template <> struct AllRightContext<Reference<Closure>>
	{
		AllRightContext(Reference<Closure>) { }
		void remember(Reference<Closure>) { }
		void restore(Reference<Closure>) { }
		static const constexpr bool haveMemory{false};
	};
	
	template <typename T, typename... Rest> struct AllRightContext<Reference<T>, Rest...>
	{
		T objCopy;
		AllRightContext<Rest...> rest;
		AllRightContext(Reference<T> ref, Rest... irest): objCopy{ref.getRef()}, rest{irest...} { }
		void remember(Reference<T> ref, Rest... irest)
		{
			if (haveMemory)
			{
				objCopy = ref.getRef();
				rest.remember(irest...);
			}
		}
		void restore(Reference<T> ref, Rest... irest)
		{
			if (haveMemory)
			{
				ref.getRef() = objCopy;
				rest.restore(irest...);
			}
		}	
		static const constexpr bool haveMemory{CheckMemory<T>::have || AllRightContext<Rest...>::haveMemory};
	};
	
	
	template <typename T> inline Reference<T> makeRef(T & target)
	{
		return Reference<T>(target);
	}
	
	// pair representing "catched" content from input
	struct Catch
	{
		uint32_t begin;
		uint32_t length;
	};
	
	// object which represent all of "catched" pairs, support for range-based FOR
	struct CatchReturn {
	protected:
		Catch * vdata;
		size_t vsize;
	public:
		size_t size() const {
			return vsize;
		}
		const Catch * data() const {
			return vdata;
		}
		CatchReturn(): vdata{nullptr}, vsize{0} { }
		CatchReturn(Catch * ldata, size_t lsize): vdata{ldata}, vsize{lsize} { }
		Catch * get(const size_t id) {
			if (id < vsize) return &vdata[id];
			else return nullptr;
		}
		Catch * begin() const {
			return vdata;
		}
		Catch * end() const {
			return (vdata + vsize);
		}
		Catch operator[](unsigned int id)
		{
			if (id < vsize) return vdata[id];
			else return Catch{0,0};
		}
	};
	
	// static-allocated memory which contains "catched" pairs
	
	template <size_t size> struct CheckMemory<StaticMemory<size>> { static const constexpr bool have = true; };
	
	template <size_t size> struct StaticMemory
	{
	protected:
		uint32_t count{0};
		Catch data[size];
	public:
		inline void reset()
		{
			count = 0;
		}
		void set(unsigned int addr, Catch content)
		{
			if (addr < size) data[addr] = content;
		}
		int add(Catch content)
		{
			if (count < size) 
			{
				unsigned int ret = count++;
				data[ret] = content;
				return ret;
			}
			return -1;
		}
		uint32_t getCount()
		{
			return count;
		}
		CatchReturn getCatches()
		{
			return CatchReturn{data, count};
		}
	};
	
	// dynamic-allocated memory (based on std::vector) which contains "catched" pairs
	
	template <> struct CheckMemory<DynamicMemory> { static const constexpr bool have = true; };
	
	struct DynamicMemory
	{
	protected:
		std::vector<Catch> data;
	public:
		inline void reset()
		{
			data = {};
		}
		void set(unsigned int addr, Catch content)
		{
			if (addr < getCount()) data[addr] = content;
		}
		int add(Catch content)
		{
			data.push_back(content);
			return data.size()-1;
		}
		size_t getCount()
		{
			return data.size();
		}
		CatchReturn getCatches()
		{
			return CatchReturn{data.data(), getCount()};
		}
	};
	
	// struct which represents Begin ^ regexp sign (matching for first-position)
	struct Begin
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (string.isBegin())
			{
				if (nright.getRef().match(string, move, deep, root, right...))
				{
					return true;
				}
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// struct which represent End $ regexp sign (matching for end-of-input)
	struct End
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t &, unsigned int, Root &, Reference<NearestRight>, Right...)
		{
			if (string.isEnd())
			{
				// there is no need for calling rest of call-chain
				return true;
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// templated struct which represent string (sequence of characters) in regexp
	template <wchar_t firstCode, wchar_t... codes> struct String<firstCode, codes...>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			size_t pos{0};
			if (checkString(string, pos, deep))
			{
				DEBUG_PRINTF("checkString match (pos = %zu)\n",pos);
				size_t pos2{0};
				if (nright.getRef().match(string.add(pos), pos2, deep, root, right...))
				{
					move = pos+pos2;
					return true;
				}
				DEBUG_PRINT("nright not match.\n");
			}
			else
			{
				DEBUG_PRINTF("String: Character '%c' is different to requested character (first: '%c') .get(). NOT match\n", *string.str, firstCode);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkString(const StringAbstraction string, size_t & move, unsigned int deep)
		{
			size_t pos{0};
			if (string.equal(firstCode))
			{
				DEBUG_PRINTF("Character '%c' compared with '%c' .get(). match\n", *string.str, firstCode);
				if (String<codes...>::checkString(string.add(1), pos, deep))
				{
					move = pos+1;
					return true;
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' compared with '%c' .get(). NOT match\n", *string.str, firstCode);
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// empty string always match if rest of callchain match
	template <> struct String<>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			return nright.getRef().match(string, move, deep, root, right...);
		}
		template <typename StringAbstraction> static inline bool checkString(const StringAbstraction, size_t &, unsigned int)
		{
			return true;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// templated struct which represent character range in regexp [a-z] or [a-zX-Y...]
	template <bool positive, wchar_t a, wchar_t b, wchar_t... rest> struct CharacterRange<positive, a, b, rest...>
	{
		static const constexpr bool isEmpty{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.getRef().match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
				else
				{
					reset(nright, right...);
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character ('%c' - '%c') .get(). NOT match\n", *string.str, a, b);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int deep)
		{
			if (string.charIsBetween(a,b)) 
			{
				DEBUG_PRINTF("Character compared with '%c'-'%c' .get(). match\n", a,b);
				return true;
			}
			else if (!CharacterRange<positive, rest...>::isEmpty && CharacterRange<positive, rest...>::checkChar(string, deep)) return true;
			else return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// empty character range which represent ANY character . sign
	template <bool positive> struct CharacterRange<positive>
	{
		static const constexpr bool isEmpty{true};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.getRef().match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
				else
				{
					reset(nright, right...);
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character .get(). NOT match\n", *string.str);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int)
		{
			if (string.exists()) 
			{
				DEBUG_PRINT("Character compared with ANY .get(). match\n");
				return true;
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};	
	
	// templated struct which represents list of chars in regexp [abcdef..] or just one character
	template <bool positive, wchar_t firstCode, wchar_t... code> struct CharacterClass<positive, firstCode, code...>
	{
		static const constexpr bool isEmpty{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.getRef().match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
				else
				{
					reset(nright, right...);
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character (first: '%c') .get(). NOT match\n", *string.str, firstCode);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int deep)
		{
			if (string.equal(firstCode)) 
			{
				DEBUG_PRINTF("Character compared with '%c' .get(). match\n", firstCode);
				return true;
			}
			else if (!CharacterClass<positive, code...>::isEmpty && CharacterClass<positive, code...>::checkChar(string, deep)) return true;
			else return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// empty character represents ANY . character
	template <bool positive> struct CharacterClass<positive>
	{
		static const constexpr bool isEmpty{true};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep)))
			{
				size_t pos{0};
				if (nright.getRef().match(string.add(1), pos, deep, root, right...))
				{
					move = pos+1;
					return true;
				}
				else
				{
					reset(nright, right...);
				}
			}
			else
			{
				DEBUG_PRINTF("Character '%c' is different to requested character .get(). NOT match\n", *string.str);
			}
			return false;
		}
		template <typename StringAbstraction> static inline bool checkChar(const StringAbstraction string, unsigned int)
		{
			if (string.exists()) 
			{
				DEBUG_PRINT("Character compared with ANY .get(). match\n");
				return true;
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// templated struct which represent catch-of-content braces in regexp, ID is unique identify of this content	
	template <unsigned int id, typename MemoryType, typename Inner, typename... Rest> struct CatchContent<id, MemoryType, Inner, Rest...>: public CatchContent<id, MemoryType, Seq<Inner,Rest...>>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			return CatchContent<id, MemoryType, Seq<Inner,Rest...>>::match(string, move, deep, root, nright, right...);
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			CatchContent<id, MemoryType, Seq<Inner,Rest...>>::reset(nright, right...);
		}
		template <unsigned int subid> inline bool get(CatchReturn & catches) 
		{
			return CatchContent<id, MemoryType, Seq<Inner,Rest...>>::template get<subid>(catches);
		}
	};
	
	// just one inner regexp variant of catch-of-content
	template <typename MemoryType> struct Mark
	{
		uint32_t begin;
		uint32_t len;
		MemoryType & memory;
		int addr{-1};
		
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			// checkpoint => set length
			len = string.getPosition() - begin;
			if (addr >= 0)
			{
				memory.set(addr,{begin,len});
			}
			else
			{
				addr = memory.add({begin,len});
			}
			return nright.getRef().match(string, move, deep, root, right...);
			
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight>, Right...)
		{
		
		}
		Mark(uint32_t lbegin, MemoryType & lmemory): begin{lbegin}, len{0}, memory(lmemory)
		{
			
		}
		Mark(const Mark & orig) = default;
		Mark & operator=(const Mark & orig)
		{
			begin = orig.begin;
			len = orig.len;
			memory = orig.memory;
			return *this;
		}
	};
	
	
	template <unsigned int id, typename MemoryType, typename Inner> struct CatchContent<id, MemoryType, Inner>: public Inner
	{
		MemoryType memory;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			Mark<MemoryType> mark{static_cast<uint32_t>(string.getPosition()), memory};
			bool ret{Inner::match(string, move, deep, root, makeRef(mark), nright, right...)};
			if (!ret)
			{
				memory.reset();
				reset(nright, right...);
			}
			return ret;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Inner::reset(nright, right...);
		}
		template <unsigned int subid> inline bool get(CatchReturn & catches) 
		{
			if (subid == id) 
			{
				//printf("here! size = %zu\n",memory.getCount());
				catches = memory.getCatches();
				return true;
			}
			else return Inner::template get<id>(catches);
		}
	};
	
	// templated struct which represents string with content from catch (not regexp anymore :) 
	// in style: ^([a-z]+)\1$ for catching string in style "abcabc" (in catch is just "abc")
	template <unsigned int baseid, unsigned int catchid> struct ReCatch
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			CatchReturn ret;
			if (root.template get<baseid>(ret)) {
				//printf("catch found (size = %zu)\n",ret.size());
				const Catch * ctch{ret.get(catchid)};
				if (ctch) {
					//printf("subcatch found\n");
					for (size_t l{0}; l != ctch->length; ++l) {
						if (!string.equalToOriginal(ctch->begin+(ctch->length-l-1),l)) return false;
					}
					size_t tmp{0};
					if (nright.getRef().match(string.add(ctch->length), tmp, deep, root, right...))
					{
						move += ctch->length + tmp;
						return true;
					}
					else
					{
						reset(nright, right...);
					}
				}
				else
				{
					//printf("subcatch NOT found %u\n",catchid);
				}
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int subid> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	template <unsigned int baseid, unsigned int catchid> struct ReCatchReverse
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			CatchReturn ret;
			if (root.template get<baseid>(ret)) {
				//printf("catch found (size = %zu)\n",ret.size());
				const Catch * ctch{ret.get(catchid)};
				if (ctch) {
					//printf("subcatch found\n");
					for (size_t l{0}; l != ctch->length; ++l) {
						if (!string.equalToOriginal(ctch->begin+l,l)) return false;
					}
					size_t tmp{0};
					if (nright.getRef().match(string.add(ctch->length), tmp, deep, root, right...))
					{
						move += ctch->length + tmp;
						return true;
					}
					else
					{
						reset(nright, right...);
					}
				}
				else
				{
					//printf("subcatch NOT found %u\n",catchid);
				}
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int subid> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// temlated struct which represent selection in regexp (a|b|c)
	template <typename FirstOption, typename... Options> struct Selection<FirstOption, Options...>: public FirstOption
	{
		Selection<Options...> rest;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (FirstOption::match(string, move, deep+1, root, nright, right...))
			{
				return true;
			}
			else
			{
				FirstOption::reset(nright, right...);
				return rest.match(string, move, deep+1, root, nright, right...);
			}
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			FirstOption::reset(nright, right...);
			rest.reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return FirstOption::template get<id>(catches) || rest.template get<id>(catches);
		}
	};
	
	// empty selection always fail
	template <> struct Selection<>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction, size_t &, unsigned int, Root &, Reference<NearestRight>, Right...)
		{
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight>, Right...)
		{
			
		}
		template <unsigned int id> inline bool get(CatchReturn &) 
		{
			return false;
		}
	};
	
	// templated struct which represent sequence of another regexps 
	template <typename First, typename... Rest> struct Sequence<First, Rest...>: public First
	{
		Sequence<Rest...> rest;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (First::match(string, move, deep, root, makeRef(rest), nright, right...))
			{
				return true;
			}
			First::reset(nright, right...);
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			First::reset(makeRef(rest), nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return First::template get<id>(catches) || rest.template get<id>(catches);
		}
	};

	// sequence of just one inner regexp
	template <typename First> struct Sequence<First>: public First
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (First::match(string, move, deep, root, nright, right...))
			{
				return true;
			}
			First::reset(nright, right...);
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			First::reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return First::template get<id>(catches);
		}
	};
	
	// templated struct which represents generic-loop with min,max limit
	// ()+ "plus" cycle have min 1 and min 0 (infinity)
	// ()* "star" cycle have min 0 and max 0 (infinity)
	template <unsigned int min, unsigned int max, typename Inner, typename... Rest> struct Repeat<min, max, Inner, Rest...>: public Repeat<min, max, Seq<Inner,Rest...>>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			return Repeat<min, max, Seq<Inner,Rest...>>::match(string, move, deep, root, nright, right...);
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Repeat<min, max, Seq<Inner,Rest...>>::reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return Repeat<min, max, Seq<Inner,Rest...>>::template get<id>(catches);
		}
	};
	
	// cycle with just one inner regexp
	template <unsigned int min, unsigned int max, typename Inner> struct Repeat<min, max, Inner>: public Inner
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			size_t pos{0};
			ssize_t lastFound{-1};
			Closure closure;
			
			Repeat<min, max, Inner> innerContext{*this};
			AllRightContext<Reference<NearestRight>, Right...> allRightContext(nright, right...);
			
			size_t tmp;
			
			for (unsigned int cycle{0}; (!max) || (cycle <= max); ++cycle)
			{
				if (nright.getRef().match(string.add(pos), tmp = 0, deep+1, root, right...) && (cycle >= min))
				{
					allRightContext.remember(nright, right...);
					nright.getRef().reset(right...);
					lastFound = pos + tmp;
					DEBUG_PRINTF(">> found at %zu\n",lastFound);
				}
				// in next expression "empty" is needed
				*this = innerContext;
				if (Inner::match(string.add(pos), tmp = 0, deep+1, root, makeRef(closure)))
				{
					innerContext = *this;
					pos += tmp;
				}
				else 
				{
					if (lastFound >= 0)
					{
						*this = innerContext;
						allRightContext.restore(nright, right...);
						DEBUG_PRINTF("cycle done (cycle = %zu)\n",cycle);
						move += static_cast<size_t>(lastFound);
						return true;
					}
					else break;
				}
				
			}
			reset(nright, right...);
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Inner::reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return Inner::template get<id>(catches);
		}
	};
	
	// wrapper for floating matching in string (begin regexp anywhere in string)
	// without Eat<...> is regexp ABC equivalent to ^ABC$
	template <typename... Inner> struct Eat: public Sequence<Inner...>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			size_t pos{0};
			
			if (!string.exists() && Sequence<Inner...>::match(string, move, deep+1, root, nright, right...))
			{
				// branch just for empty strings
				return true;
			}
			else while (string.exists(pos)) {
				size_t imove{0};
				//DEBUG_PRINTF("eating... (pos = %zu)\n",pos);
				if (Sequence<Inner...>::match(string.add(pos), imove, deep+1, root, nright, right...))
				{
					move += pos + imove;
					return true;
				}
				else pos++;
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Sequence<Inner...>::reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return Sequence<Inner...>::template get<id>(catches);
		}
	};
	
	// debug template
	template <unsigned int part, typename... Inner> struct Debug: Sequence<Inner...>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (Sequence<Inner...>::match(string, move, deep, root, nright, right...))
			{
				std::cout << "[part "<<part<<": match; move="<<move<<"; right="<<sizeof...(Right)<<"]\n";
				return true;
			}
			else
			{
				std::cout << "[part "<<part<<": no match; move="<<move<<"; right="<<sizeof...(Right)<<"]\n";
				return false;
			}
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Sequence<Inner...>::reset(nright, right...);
		}
		template <unsigned int id> inline bool get(CatchReturn & catches) 
		{
			return Sequence<Inner...>::template get<id>(catches);
		}
	};
	
	// templated struct which contains regular expression and is used be user :)
	template <typename... Definition> struct RegularExpression
	{
		Eat<Definition...> eat;
		template <CompareFnc<char> compare = charactersAreEqual<char>> inline bool operator()(std::string string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const char *, const char, compare>(string.c_str()), pos, 0, eat, makeRef(closure));
		}
		template <CompareFnc<char> compare = charactersAreEqual<char>> inline bool operator()(const char * string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const char *, const char, compare>(string), pos, 0, eat, makeRef(closure));
		}
		template <CompareFnc<wchar_t> compare = charactersAreEqual<wchar_t>> inline bool operator()(std::wstring string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const wchar_t *, const wchar_t, compare>(string.c_str()), pos, 0, eat, makeRef(closure));
		}
		template <CompareFnc<wchar_t> compare =  charactersAreEqual<wchar_t>> inline bool operator()(const wchar_t * string)
		{
			size_t pos{0};
			Closure closure;
			return eat.match(StringAbstraction<const wchar_t *, const wchar_t, compare>(string), pos, 0, eat, makeRef(closure));
		}
		template <CompareFnc<char> compare = charactersAreEqual<char>> inline bool match(std::string string)
		{
			return operator()<compare>(string);
		}
		template <CompareFnc<char> compare = charactersAreEqual<char>> inline bool match(const char * string)
		{
			return operator()<compare>(string);
		}
		template <CompareFnc<wchar_t> compare = charactersAreEqual<wchar_t>> inline bool match(std::wstring string)
		{
			return operator()<compare>(string);
		}
		template <CompareFnc<wchar_t> compare = charactersAreEqual<wchar_t>> inline bool match(const wchar_t * string)
		{
			return operator()<compare>(string);
		}
		template <unsigned int id> inline CatchReturn getCatch()
		{
			CatchReturn catches;
			eat.template get<id>(catches);
			return catches;
		}
		template <unsigned int id, typename StringType> inline auto part(const StringType string, unsigned int subid = 0) -> decltype(string)
		{
			return string.substr(getCatch<id>()[subid].begin, getCatch<id>()[subid].length);
		}
	};
}

#endif