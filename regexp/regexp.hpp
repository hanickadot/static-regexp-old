// The MIT License (MIT)
// 
// Copyright (c) 2014-2015 Hana Dusíková (hanicka@hanicka.net)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef __REGEXP__REGEXP__HPP__
#define __REGEXP__REGEXP__HPP__

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

namespace SRX {
	
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
	
	// identifier
	template <unsigned int key, unsigned int value> struct Identifier;
	
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
	template <unsigned int key, unsigned int value> using Id = Identifier<key,value>;
	using Space = Chr<' '>;
	using WhiteSpace = Set<' ','\t','\r','\n'>;
	using Number = CRange<'0','9'>;
	
	
	
	template <typename CharType> using CompareFnc = bool (*)(const CharType, const CharType, const CharType);
	
	// traits
	
	template <typename T> struct IsSequence { static constexpr bool value = false; };
	template <typename T> struct IsDynamicMemory { static constexpr bool value = false; };
	template <typename T> struct IsStaticMemory { static constexpr size_t value = 0; };
	
	// implementation:
	
	template <typename T, typename... Rest> struct RightContextHelper
	{
		static std::ostream & print(std::ostream & str, T t, Rest... rest)
		{
			RightContextHelper<T>::print(str, t);
			str << ',';
			return RightContextHelper<Rest...>::print(str,rest...);
		}
	};
	
	template <typename T> struct RightContextHelper<T>
	{
		static std::ostream & print(std::ostream & str, T t)
		{
			t.toString(str);
			return str;
		}
	};
	
	template <typename T, typename... Rest> std::ostream & printRightContext(const char * description, std::ostream & str, T t, Rest... rest)
	{
		str << description << ": ";
		RightContextHelper<T,Rest...>::print(str,t,rest...);
		//printRightContextInner<T, Rest...>(str,t,rest...);
		str << "\n";
		return str;
	}
	
	
	
	template <typename T> struct Reference
	{
		T & target;
		inline Reference(T & ltarget): target(ltarget) { }
		inline T & getRef() { return target; }
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			return target.template toString<envelope>(str);
		}
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
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		inline void reset()
		{
			
		}
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			str << " Closure";
			return str;
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
		const Catch * vdata;
		size_t vsize;
	public:
		size_t size() const {
			return vsize;
		}
		const Catch * data() const {
			return vdata;
		}
		CatchReturn(): vdata{nullptr}, vsize{0} { }
		CatchReturn(const Catch * ldata, size_t lsize): vdata{ldata}, vsize{lsize} { }
		const Catch * get(const size_t id) {
			if (id < vsize) return &vdata[id];
			else return nullptr;
		}
		const Catch * begin() const {
			return vdata;
		}
		const Catch * end() const {
			return (vdata + vsize);
		}
		const Catch operator[](unsigned int id)
		{
			if (id < vsize) return vdata[id];
			else return Catch{0,0};
		}
	};
	
	template <unsigned int id, typename T, typename... Tx> inline bool getCatchFromSubrexpHelper(CatchReturn & catches, T & from, Tx &... next)
	{
		if (!from.template getCatch<id>(catches))
		{
			return getCatchFromSubrexpHelper(catches, next...);
		}
		else
		{
			return true;
		}
	}
	
	template <unsigned int id, typename T> inline bool getCatchFromSubrexpHelper(CatchReturn & catches, T & from)
	{
		return from.template getCatch<id>(catches);
	}
	
	template <unsigned int id, typename T, typename... Tx> inline CatchReturn getCatchFromSubrexp(T & from, Tx &... next)
	{
		CatchReturn catches;
		getCatchFromSubrexpHelper<id>(catches, from, next...);
		return catches;
	}
	
	// static-allocated memory which contains "catched" pairs
	
	template <size_t size> struct CheckMemory<StaticMemory<size>> { static const constexpr bool have = true; };
	
	template <size_t size> struct IsStaticMemory<StaticMemory<size>> { static constexpr size_t value = size; };
	
	template <size_t size> struct StaticMemory
	{
	protected:
		uint32_t count{0};
		Catch data[size];
	public:
		template <unsigned int> inline void reset()
		{
			count = 0;
		}
		StaticMemory & operator=(const StaticMemory & right)
		{
			count = right.count;
			for (unsigned int i{0}; i != count; ++i)
			{
				data[i] = right.data[i];
			}
			return *this;
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
		uint32_t getCount() const
		{
			return count;
		}
		CatchReturn getCatches() const
		{
			return CatchReturn{data, count};
		}
		template <size_t rsize> void merge(StaticMemory<rsize> & right)
		{
			const unsigned int origCount{count};
			for (unsigned int i{origCount}; i < size; ++i)
			{
				if (i-origCount < right.count)
				{
					data[i] = right.data[i-origCount];
					count++;
				}
				else break;
			}
		}
	};
	
	// dynamic-allocated memory (based on std::vector) which contains "catched" pairs
	
	template <> struct CheckMemory<DynamicMemory> { static const constexpr bool have = true; };
	
	template <> struct IsDynamicMemory<DynamicMemory> { static const constexpr bool value = true; };
	
	struct DynamicMemory
	{
	protected:
		std::vector<Catch> data;
	public:
		template <unsigned int id> inline void reset()
		{
			fprintf(stderr,"Reseting %u (%p)...\n",id,this);
			for (const auto & c: data) fprintf(stderr,"Reset %u -> %u..%u\n",id,c.begin,c.length);
			data.resize(0);
		}
		DynamicMemory & operator=(const DynamicMemory & right)
		{
			data = right.data;
			return *this;
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
		size_t getCount() const
		{
			return data.size();
		}
		CatchReturn getCatches() const
		{
			return CatchReturn{data.data(), getCount()};
		}
		void merge(DynamicMemory & right)
		{
			for (const auto it: right.data)
			{
				data.push_back(it);
			}
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			return str << "Begin";
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			return str << "End";
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope) str << "String<\"";
			str << (char)firstCode;
			String<codes...> tmp;
			tmp.template toString<false>(str);
			if (envelope) str << "\">";
			return str;
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope) str << "String<\"\">";
			return str;
		}
	};
	
	// templated struct which represent character range in regexp [a-z] or [a-zX-Y...]
	template <bool positive, wchar_t a, wchar_t b, wchar_t... rest> struct CharacterRange<positive, a, b, rest...>
	{
		static const constexpr bool isEmpty{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep) && !string.isEnd()))
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope)
			{
				str << "[";
				if (!positive) str << '^';
			}
			str << (char)a << '-' << (char)b;
			CharacterRange<positive,rest...> tmp;
			tmp.template toString<false>(str);
			if (envelope) str << "]";
			return str;
		}
	};
	
	// empty character range which represent ANY character . sign
	template <bool positive> struct CharacterRange<positive>
	{
		static const constexpr bool isEmpty{true};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep) && !string.isEnd()))
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope)
			{
				str << "CharacterRange";
			}
			return str;
		}
	};	
	
	// templated struct which represents list of chars in regexp [abcdef..] or just one character
	template <bool positive, wchar_t firstCode, wchar_t... code> struct CharacterClass<positive, firstCode, code...>
	{
		static const constexpr bool isEmpty{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep) && !string.isEnd()))
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope)
			{
				str << "[";
				if (!positive) str << '^';
			}
			str << (char)firstCode;
			CharacterClass<positive,code...> tmp;
			tmp.template toString<false>(str);
			if (envelope) str << "]";
			return str;
		}
	};
	
	// empty character represents ANY . character
	template <bool positive> struct CharacterClass<positive>
	{
		static const constexpr bool isEmpty{true};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if ((positive && checkChar(string, deep)) || (!positive && !checkChar(string, deep) && !string.isEnd()))
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope)
			{
				str << "CharacterClass";
			}
			return str;
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
		template <unsigned int subid> inline bool getCatch(CatchReturn & catches) const
		{
			return CatchContent<id, MemoryType, Seq<Inner,Rest...>>::template getCatch<subid>(catches);
		}
		template <unsigned int key> inline unsigned int getIdentifier() const
		{
			return CatchContent<id, MemoryType, Seq<Inner,Rest...>>::template getIdentifier<key>();
		}
	};
	
	// just one inner regexp variant of catch-of-content
	template <typename MemoryType, unsigned int id> struct Mark
	{
		uint32_t begin;
		uint32_t len;
		MemoryType memory;
		int addr{-1};
		
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			// checkpoint => set length
			len = string.getPosition() - begin;
			if (addr >= 0)
			{
				memory.set(addr,{begin,len});
				fprintf(stderr,"mark %u set '%.*s' %u..%u (addr=%d)\n",id,len,string.original+begin,begin,len,addr);
			}
			else
			{
				addr = memory.add({begin,len});
				fprintf(stderr,"mark %u added '%.*s' %u..%u (addr=%d)\n",id,len,string.original+begin,begin,len,addr);
			}
			return nright.getRef().match(string, move, deep, root, right...);
			
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		Mark(uint32_t lbegin): begin{lbegin}, len{0}
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
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			str << "Mark<" << id << ">";
			return str;
		}
	};
	
	
	template <unsigned int id, typename MemoryType, typename Inner> struct CatchContent<id, MemoryType, Inner>: public Inner
	{
		MemoryType memory;
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			Mark<MemoryType, id> mark{static_cast<uint32_t>(string.getPosition())};
			bool ret{Inner::match(string, move, deep, root, makeRef(mark), nright, right...)};
			if (!ret)
			{
				reset(nright, right...);
			}
			else
			{
				memory.merge(mark.memory);
			}
			return ret;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			memory.template reset<id>();
			Inner::reset(nright, right...);
		}
		template <unsigned int subid> inline bool getCatch(CatchReturn & catches) const
		{
			if (subid == id) 
			{
				catches = memory.getCatches();
				return true;
			}
			else return Inner::template getCatch<subid>(catches);
		}
		template <unsigned int key> inline unsigned int getIdentifier() const
		{
			return Inner::template getIdentifier<key>();
		}
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			if (IsDynamicMemory<MemoryType>::value) str << "DynamicCatch<" << id << ", ";
			else if (IsStaticMemory<MemoryType>::value) str << "StaticCatch<" << id << ", size("<<IsStaticMemory<MemoryType>::value<<"), ";
			else str << "CatchContent<" << id << ", ";
			if (IsSequence<Inner>::value) Inner::template toString<false>(str);
			else Inner::template toString<true>(str);
			str << ">";
			return str;
		}
	};
	
	// templated struct which represents string with content from catch (not regexp anymore :) 
	// in style: ^([a-z]+)\1$ for catching string in style "abcabc" (in catch is just "abc")
	template <unsigned int baseid, unsigned int catchid> struct ReCatch
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			CatchReturn ret;
			if (root.template getCatch<baseid>(ret)) {
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
		template <bool> std::ostream & toString(std::ostream & str) const
		{
			str << "ReCatch<" << baseid << ">";
			return str;
		}
	};
	
	template <unsigned int baseid, unsigned int catchid> struct ReCatchReverse
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			CatchReturn ret;
			if (root.template getCatch<baseid>(ret)) {
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
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
	};
	
	// identify path thru regexp
	template <unsigned int key, unsigned int value> struct Identifier
	{
		bool matched{false};
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (nright.getRef().match(string, move, deep, root, right...))
			{
				matched = true;
				return true;
			}
			else 
			{
				matched = false;
				return false;
			}
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			matched = false;
			nright.getRef().reset(right...);
		}
		template <unsigned int> inline bool getCatch(CatchReturn &) const
		{
			return false;
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return matched ? (key == rkey ? value : 0) : 0;
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
			Closure closure;
			FirstOption::reset(nright, makeRef(closure));
			rest.reset(nright, right...);
		}
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return FirstOption::template getCatch<id>(catches) || rest.template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			if (FirstOption::template getIdentifier<rkey>()) return FirstOption::template getIdentifier<rkey>();
			else return rest.template getIdentifier<rkey>();
		}
	};
	
	// empty selection always fail
	template <> struct Selection<>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction, size_t &, unsigned int, Root &, Reference<NearestRight>, Right...)
		{
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			nright.getRef().reset(right...);
		}
		template <unsigned int> inline bool getCatch(CatchReturn &) const 
		{
			return false;
		}
		template <unsigned int> inline unsigned int getIdentifier() const
		{
			return 0;
		}
	};
	
	// templated struct which represent sequence of another regexps 
	
	template <typename First, typename... Rest> struct IsSequence<Sequence<First,Rest...>> { static constexpr bool value = true; };
	
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
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return First::template getCatch<id>(catches) || rest.template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			if (First::template getIdentifier<rkey>()) return First::template getIdentifier<rkey>();
			else return rest.template getIdentifier<rkey>();
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope) str << "Sequence<";
			First::template toString<true>(str);
			str << ", ";
			rest.template toString<false>(str);
			if (envelope) str << ">";
			return str;
		}
	};

	// sequence of just one inner regexp
	template <typename First> struct IsSequence<Sequence<First>> { static constexpr bool value = true; };
	
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
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return First::template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return First::template getIdentifier<rkey>();
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope) str << "Sequence<";
			if (IsSequence<First>::value) First::template toString<false>(str);
			else First::template toString<true>(str);
			if (envelope) str << ">";
			return str;
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
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return Repeat<min, max, Seq<Inner,Rest...>>::template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return Repeat<min, max, Seq<Inner,Rest...>>::template getIdentifier<rkey>();
		}
	};
	
	// cycle with just one inner regexp
	
	struct RepeatCounter
	{
		static unsigned int getID()
		{
			static unsigned int id = 0;
			return ++id;
		}
	};
	
	template <unsigned int min, unsigned int max, typename Inner> struct Repeat<min, max, Inner>: public Inner
	{
		unsigned int id{0};
		Repeat(): id{RepeatCounter::getID()}
		{
			
		}
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
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return Inner::template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return Inner::template getIdentifier<rkey>();
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope)
			{
				if (min == 1 && max == 0) str << "Plus<";
				else if (min == 0 && max == 0) str << "Star<";
				else str << "Repeat<" << min << ',' << max << ',';
			}
			if (IsSequence<Inner>::value) Inner::template toString<false>(str);
			else Inner::template toString<true>(str);
			if (envelope) str << ">";
			return str;
		}
	};
	
	// wrapper for floating matching in string (begin regexp anywhere in string)
	// without Eat<...> is regexp ABC equivalent to ^ABC
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
				else
				{
					reset(nright, right...);
					pos++;
				}
			}
			return false;
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Sequence<Inner...>::reset(nright, right...);
		}
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return Sequence<Inner...>::template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return Sequence<Inner...>::template getIdentifier<rkey>();
		}
		template <bool envelope = true> std::ostream & toString(std::ostream & str) const
		{
			if (envelope) str << "Eat<";
			Sequence<Inner...>::template toString<false>(str);
			if (envelope) str << ">";
			return str;
		}
	};
	
	// debug template
	template <unsigned int part, typename... Inner> struct Debug: Sequence<Inner...>
	{
		template <typename StringAbstraction, typename Root, typename NearestRight, typename... Right> inline bool match(const StringAbstraction string, size_t & move, unsigned int deep, Root & root, Reference<NearestRight> nright, Right... right)
		{
			if (Sequence<Inner...>::match(string, move, deep, root, nright, right...))
			{
				std::cout << "[part "<<part<<": match; pos="<<string.getPosition()<<"; move="<<move<<"; right="<<sizeof...(Right)<<"]\n";
				return true;
			}
			else
			{
				std::cout << "[part "<<part<<": no match; pos="<<string.getPosition()<<"; move="<<move<<"; right="<<sizeof...(Right)<<"]\n";
				return false;
			}
		}
		template <typename NearestRight, typename... Right> inline void reset(Reference<NearestRight> nright, Right... right)
		{
			Sequence<Inner...>::reset(nright, right...);
		}
		template <unsigned int id> inline bool getCatch(CatchReturn & catches) const
		{
			return Sequence<Inner...>::template getCatch<id>(catches);
		}
		template <unsigned int rkey> inline unsigned int getIdentifier() const
		{
			return Sequence<Inner...>::template getIdentifier<rkey>();
		}
	};
	
	// templated struct which contains regular expression and is used be user :)
	template <typename... Definition> struct RegularExpression
	{
		Eat<Definition...> eat;
		void reset()
		{
			Closure closure;
			eat.reset(makeRef(closure));
		}
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
		template <unsigned int key> unsigned int getIdentifier()
		{
			return eat.template getIdentifier<key>();
		}
		template <unsigned int id> inline CatchReturn getCatch()
		{
			CatchReturn catches;
			eat.template getCatch<id>(catches);
			return catches;
		}
		template <unsigned int id, typename StringType> inline auto part(const StringType string, unsigned int subid = 0) -> decltype(string)
		{
			return string.substr(getCatch<id>()[subid].begin, getCatch<id>()[subid].length);
		}
		std::ostream & toString(std::ostream & str) const
		{
			return eat.toString(str);
		}
	};
	
	template <typename... Definition> std::ostream & operator<<(std::ostream & str, const RegularExpression<Definition...> & regexp)
	{
		return regexp.toString(str);
	}
	
	template <typename... Definition> struct RegularExpression2
	{
		Sequence<Definition...> eat;
		void reset()
		{
			Closure closure;
			eat.reset(makeRef(closure));
		}
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
		template <unsigned int key> unsigned int getIdentifier()
		{
			return eat.template getIdentifier<key>();
		}
		template <unsigned int id> inline CatchReturn getCatch()
		{
			CatchReturn catches;
			eat.template getCatch<id>(catches);
			return catches;
		}
		template <unsigned int id, typename StringType> inline auto part(const StringType string, unsigned int subid = 0) -> decltype(string)
		{
			return string.substr(getCatch<id>()[subid].begin, getCatch<id>()[subid].length);
		}
		std::ostream & toString(std::ostream & str) const
		{
			return eat.toString(str);
		}
	};
	
	template <typename... Definition> std::ostream & operator<<(std::ostream & str, const RegularExpression2<Definition...> & regexp)
	{
		return regexp.toString(str);
	}
}

#endif