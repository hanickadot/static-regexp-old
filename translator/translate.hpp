#ifndef __TRANSLATOR_TRANSLATE_HPP__
#define __TRANSLATOR_TRANSLATE_HPP__

#include <functional>
#include <vector>
#include <stack>
#include <list>
#include <sstream>
#include <ctype.h>
#include <set>

class RegExpCompiler
{
protected:
	struct StackError{};
	struct Part
	{
		virtual ~Part() {}
		virtual void print() {};
		virtual const char * type() {return "part";}
		virtual void createRegexp(std::stringstream &, bool) { };
	};
	using PartStack = std::stack<Part*>;
	using PartVector = std::vector<Part*>;
	struct SeqStart: public Part
	{
		virtual ~SeqStart() {}
		const char * id{"unknown-part"};
		virtual void print() { printf("%s",id); };
		virtual const char * type() {return "SeqStart";}
		SeqStart(const char * lid): id{lid}
		{
			
		}
	};
	struct Begin: public Part
	{
		virtual ~Begin() {}
		virtual void print() { printf("begin"); };
		virtual const char * type() {return "Begin";}
		Begin()
		{
			
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Begin";
		}
	};
	struct Any: public Part
	{
		virtual ~Any() {}
		virtual void print() { printf("any"); };
		virtual const char * type() {return "Any";}
		Any()
		{
			
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Any";
		}
	};
	struct End: public Part
	{
		virtual ~End() {}
		virtual void print() { printf("end"); };
		virtual const char * type() {return "Begin";}
		End()
		{
			
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "End";
		}
	};
	struct OptionStart: public Part
	{
		virtual ~OptionStart() {}
		const char * id{"unknown-part"};
		virtual void print() { printf("%s",id); };
		virtual const char * type() {return "OptionStart";}
		OptionStart(const char * lid): id{lid}
		{
			
		}
	};
	struct Sequence: public Part
	{
		virtual ~Sequence() {}
		PartVector parts;
		virtual const char * type() {return "Sequence";}
		Sequence(PartVector lparst): parts{std::move(lparst)}
		{
			
		}
		virtual void print()
		{
			printf("(");
			bool first{true};
			for (auto * part: parts)
			{
				if (first) first = false;
				else printf(",");
				part->print();
			}
			printf(")");
		}
		void workOut(PartVector & tmp, Part * & last, Part * current)
		{
			Character * cc = dynamic_cast<Character*>(current);
			if (Character * chr = dynamic_cast<Character*>(last))
			{
				if (cc)
				{
					last = new CharString(chr->c, cc->c);
					delete chr;
					delete cc;
				}
				else
				{
					tmp.push_back(last);
					last = current;
				}
			}
			else if (CharString * sch = dynamic_cast<CharString*>(last))
			{
				if (cc)
				{
					sch->str.push_back(cc->c);
				}
				else 
				{
					tmp.push_back(last);
					last = current;
				}
			}
			else
			{
				tmp.push_back(last);
				last = current;
			}
		}
		void optimize()
		{
			PartVector tmp;
			bool first{true};
			Part * last{nullptr};
			for (auto * part: parts)
			{
				if (first) 
				{
					last = part;
					first = false;
				}
				else 
				{
					workOut(tmp,last,part);
				}
			}
			if (last)
			{
				workOut(tmp,last,nullptr);
			}
			parts = tmp;
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			optimize();
			if (ns) ss << "SRegExp::";
			bool first{true};
			ss << "Seq<";
			for (auto * part: parts)
			{
				if (first) first = false;
				else ss << ',';
				part->createRegexp(ss,ns);
			}
			ss << ">";
		}
	};
	struct CharString: public Part
	{
		virtual ~CharString() {}
		std::vector<char> str;
		virtual const char * type() {return "CharString";}
		CharString(char a, char b): str{{a,b}}
		{
			
		}
		virtual void print()
		{
			bool first{true};
			printf("\"");
			for (auto c: str)
			{
				if (first) first = false;
				else printf(",");
				printf("'%c'",c);
			}
			printf("\"");
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Str<";
			bool first{true};
			for (auto c: str)
			{
				if (first) first = false;
				else ss << ',';
				if (std::isalpha(c)) ss << "'" << c << "'";
				else ss << "0x" << std::hex << (unsigned int)c;
			}
			ss << ">";
			
		}
	};
	struct Character: public Part
	{
		virtual ~Character() {}
		char c;
		virtual const char * type() {return "Character";}
		Character(char lc): c{lc}
		{
			
		}
		virtual void print()
		{
			printf("'%c'",c);
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			if (std::isalpha(c)) ss << "Chr<'" << c << "'>";
			else ss << "Chr<0x" << std::hex << (unsigned int)c << ">";
		}
	};
	using CPair = std::pair<char,char>;
	struct CharSet: public Part
	{
		virtual ~CharSet() {}
		bool positive;
		std::set<char> cset;
		virtual const char * type() {return "CharSet";}
		CharSet(bool pos): positive{pos}
		{
			
		}
		virtual void print()
		{
			printf("{set: ");
			bool first{true};
			for (auto c: cset)
			{
				if (first) first = false;
				else printf(",");
				printf("'%c'",c);
			}
			printf("}");
		}
		void add(char a, char b)
		{
			for (char c{a}; c <= b; ++c) cset.insert(c);
		}
		std::set<CPair> normalize()
		{
			std::set<CPair> out;
			char a,b;
			bool empty = true;
			for (char c: cset)
			{
				if (empty) 
				{
					a = b = c;
					empty = false;
				}
				else 
				{
					if (c == b+1) b = c;
					else
					{
						out.insert({a,b});
						a = b = c;
					}
				}
			}
			if (!empty) out.insert({a,b});
			return out;
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			normalize();
			std::set<CPair> calcset = normalize();
			if (ns) ss << "SRegExp::";
			if (positive) ss << "CSet<";
			else ss << "CNegSet<";
			bool first{true};
			for (auto c: calcset)
			{
				if (first) first = false;
				else ss << ',';
				if (c.second == 0) 
				{
					if (std::isalpha(c.first)) ss << "'" << c.first << "','" << c.first << "'";
					else ss << "0x" << std::hex << (unsigned int)c.first << ',' << "0x" << std::hex << (unsigned int)c.first;
				}
				else 
				{
					if (std::isalpha(c.first)) ss << "'" << c.first << "',";
					else ss << "0x" << std::hex << (unsigned int)c.first << ",";
					
					if (std::isalpha(c.second)) ss << "'" << c.second << "'";
					else ss << "0x" << std::hex << (unsigned int)c.second << "";
				}
			}
			ss << ">";
		}
	};
	struct Option: public Part
	{
		virtual ~Option() {}
		std::list<Part*> parts;
		virtual const char * type() {return "Option";}
		Option(Part * left, Part * right): parts{{left,right}}
		{
			
		}
		void addPart(Part * part)
		{
			parts.push_back(part);
		}
		virtual void print()
		{
			printf("{opt: ");
			bool first{true};
			for (auto * part: parts)
			{
				if (first) first = false;
				else printf("|");
				part->print();
			}
			printf("}");
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			bool first{true};
			ss << "Sel<";
			for (auto * part: parts)
			{
				if (first) first = false;
				else ss << ',';
				part->createRegexp(ss,ns);
			}
			ss << ">";
		}
	};
	struct Star: public Part
	{
		virtual ~Star() {}
		virtual const char * type() {return "Star";}
		Part * what;
		Part * rest{nullptr};
		Star(Part * lwhat): what{lwhat}
		{
			
		}
		virtual void print()
		{
			printf("{");
			what->print();
			printf("}* ");
			if (rest)
			{
				printf("{");
				rest->print();
				printf("}");
			}
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Star<";
			what->createRegexp(ss,ns);
			if (rest)
			{
				ss << ", ";
				rest->createRegexp(ss,ns);
			}
			ss << ">";
		}
	};
	struct Optional: public Part
	{
		virtual ~Optional() {}
		virtual const char * type() {return "Optional";}
		Part * what;
		Part * rest{nullptr};
		Optional(Part * lwhat): what{lwhat}
		{
			
		}
		virtual void print()
		{
			printf("{");
			what->print();
			printf("}? ");
			if (rest)
			{
				printf("{");
				rest->print();
				printf("}");
			}
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Opt<";
			what->createRegexp(ss,ns);
			if (rest)
			{
				ss << ", ";
				rest->createRegexp(ss,ns);
			}
			ss << ">";
		}
	};
	struct Plus: public Part
	{
		virtual ~Plus() {}
		virtual const char * type() {return "Plus";}
		Part * what;
		Part * rest{nullptr};
		Plus(Part * lwhat): what{lwhat}
		{
			
		}
		virtual void print()
		{
			printf("{");
			what->print();
			printf("}+ ");
			if (rest)
			{
				printf("{");
				rest->print();
				printf("}");
			}
		}
		virtual void createRegexp(std::stringstream & ss, bool ns)
		{
			if (ns) ss << "SRegExp::";
			ss << "Plus<";
			what->createRegexp(ss,ns);
			if (rest)
			{
				ss << ", ";
				rest->createRegexp(ss,ns);
			}
			ss << ">";
		}
	};
	struct EndOfInput {};
	template <typename CharType> struct InternalState
	{
	protected:
		using CharFunction = std::function<CharType()>;
		CharFunction getCharInternal;
		CharType currentChar;
		bool eoi{false};
		std::stack<Part *> stack;
	public:
		CharType readChar()
		{
			try
			{
				return currentChar = getCharInternal();
			}
			catch (EndOfInput)
			{
				eoi = true;
				return currentChar;
			}
		}
		inline CharType getCurrentChar() const
		{
			return currentChar;
		}
		inline bool isEOI() const
		{
			return eoi;
		}
		inline bool controlChar() const
		{
			switch (currentChar)
			{
				case '^': case '$': case '(': case ')': case '[': case ']': case '+': case '*': case '?': case '\\': case '|': case '.':
					return true;
				default:
					return false;
			}
		}
		inline bool checkToken(char c, const char *)
		{
			if (currentChar == c)
			{
				readChar();
				return true;
			}
			return false;
		}
		inline void output(std::string str)
		{
			printf("%s\n",str.c_str());
		}
		template <bool dummy = false> void insertIntoSet(char c)
		{
			if (!dummy)
			{
				CharSet * cs = dynamic_cast<CharSet*>(stack.top());
				if (cs) cs->add(c,c);
			}
		}
		void insertIntoSet2(char c, char d)
		{
			CharSet * cs = dynamic_cast<CharSet*>(stack.top());
			if (cs) cs->add(c,d);
		}
		void insertSet(bool positive)
		{
			stack.push(new CharSet(positive));
		}
		void insertCharacter(char c)
		{
			stack.push(new Character(c));
		}
		template <typename T> void insertRepeat()
		{
			//printf("insertRepeat:");
			Part * last = stack.top();
			//last->print();
			//printf("\n");
			stack.pop();
			stack.push(new T(last));
		}
		void insertOptional()
		{
			//printf("insertRepeat:");
			Part * last = stack.top();
			//last->print();
			//printf("\n");
			stack.pop();
			stack.push(new Optional(last));
		}
		template <typename T> void insertEndRepeat()
		{
			PartVector tmp;
			while (!stack.empty() && !dynamic_cast<T*>(stack.top()))
			{
				tmp.insert(tmp.begin(),stack.top());
				stack.pop();
			}
			T * parent = dynamic_cast<T*>(stack.top());
			if (parent && tmp.size()) parent->rest = createFromVector<Sequence>(tmp);
			if (!parent) printf("bad!!\n");
		}
		void insertBegin()
		{
			stack.push(new Begin());
		}
		void insertAny()
		{
			stack.push(new Any());
		}
		void insertEnd()
		{
			stack.push(new End());
		}
		void insertOption()
		{
			insertStart<OptionStart>("option");
		}
		void insertEndOfOption()
		{
			OptionStart * block;
			SeqStart * seqblock;
			Part * right = getSeq<OptionStart>(block);
			Part * left = getSeq<SeqStart>(seqblock);
			stack.push(seqblock);
			if (block) delete block;
		
			if (right && left)
			{
				if (Option * opt = dynamic_cast<Option*>(left))
				{
					stack.push(left);
					opt->addPart(right);
				}
				else
				{
					stack.push(new Option(left,right));
				}
			}
		}
		template <typename T> Part * createFromVector(std::vector<Part*> & vec)
		{
			if (vec.size() == 1)
			{
				return *vec.begin();
			}
			else if (vec.size())
			{
				return static_cast<Part*>(new T(std::move(vec)));
			}
			else
			{
				return nullptr;
			}
		}
		template <typename T> void insertStart(const char * id = "unknown")
		{
			stack.push(new T(id));
		}
		template <typename T> Part * getSeq(T * & obj)
		{
			PartVector tmp;
			while (!stack.empty() && !dynamic_cast<T*>(stack.top()))
			{
				tmp.insert(tmp.begin(),stack.top());
				stack.pop();
			}
			if (!stack.empty() && dynamic_cast<T*>(stack.top())) 
			{
				obj = static_cast<T*>(stack.top());
				stack.pop();
			}
			else throw StackError();
			if (tmp.size())
			{
				return createFromVector<Sequence>(tmp);
			}
			return nullptr;
		}
		Part * getOptSeq(Part * & obj)
		{
			PartVector tmp;
			while (!stack.empty() && !dynamic_cast<SeqStart*>(stack.top()) && !dynamic_cast<OptionStart*>(stack.top()))
			{
				tmp.insert(tmp.begin(),stack.top());
				stack.pop();
			}
			if (!stack.empty() && (dynamic_cast<SeqStart*>(stack.top()) || dynamic_cast<OptionStart*>(stack.top()))) 
			{
				obj = stack.top();
				stack.pop();
			}
			else throw StackError();
			if (tmp.size())
			{
				return createFromVector<Sequence>(tmp);
			}
			return nullptr;
		}
		void insertSeq()
		{
			SeqStart * block;
			Part * p = getSeq<SeqStart>(block);
			
			if (block) delete block;
			
			if (p)
			{
				stack.push(p);
			}
		}
		void inspectStack()
		{
			PartStack tmp;
			int level{0};
			while (!stack.empty())
			{
				printf("%d: ",level--);
				printf("%s: ",stack.top()->type());
				stack.top()->print();
				printf("\n");
				tmp.push(stack.top());
				stack.pop();
			}
			while (!tmp.empty())
			{
				stack.push(tmp.top());
				tmp.pop();
			}
		}
		InternalState(CharFunction charFunction): getCharInternal{charFunction}
		{
			
		}
		
		bool start()
		{
			insertStart<SeqStart>("start");
			readChar();
			bool ret = nonterm_s(true);
			insertSeq();
			return ret;
		}
		
		std::string createRegexp(bool ns = false)
		{
			std::stringstream ss;
			if (!stack.empty()) 
			{
				stack.top()->createRegexp(ss,ns);
			}
			return ss.str();
		}
		
		bool nonterm_s(bool enabled) {
			//output("nonterm_s");
			if (!nonterm_string(enabled)) return false;
			if (!nonterm_s_sec(enabled)) return false;
			return true;
		}

		bool nonterm_a(bool enabled) {
			//output("nonterm_a");
			if ((!isEOI() && (!controlChar())) || (!isEOI() && (currentChar == ')')) || (!isEOI() && (currentChar == '$')) || (!isEOI() && (currentChar == '.')) || (!isEOI() && (currentChar == '\\')) || (!isEOI() && (currentChar == '[')) || (!isEOI() && (currentChar == '(')) || (!isEOI() && (currentChar == '^')) || (!isEOI() && (currentChar == '|')) || (isEOI())) {
				if (!nonterm_string(enabled)) return false;
				return true;
			}
			else if ((!isEOI() && (currentChar == '*'))) {
				readChar();
				insertRepeat<Star>();
				//if (enabledNow && semantics) semantics->repeat(enabled);
				if (!nonterm_string(enabled)) return false;
				insertEndRepeat<Star>();
				return true;
			}
			else if ((!isEOI() && (currentChar == '?'))) {
				readChar();
				insertOptional();
				//if (enabledNow && semantics) semantics->repeat(enabled);
				if (!nonterm_string(enabled)) return false;
				insertEndRepeat<Optional>();
				return true;
			}
			else if ((!isEOI() && (currentChar == '+'))) {
				readChar();
				insertRepeat<Plus>();
				//if (enabledNow && semantics) semantics->plus(enabled);
				if (!nonterm_string(enabled)) return false;
				insertEndRepeat<Plus>();
				return true;
			}
			else {
				//error 
				if (isEOI()) {
					//errorEnd(currentChar,"<a>","TOKEN_A, TOKEN_C, ')', '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '+', '*', '^', '|', TOKEN_U, end of input");
				}
				else {
					//errorUnExpected(currentChar,"<a>","TOKEN_A, TOKEN_C, ')', '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '+', '*', '^', '|', TOKEN_U, end of input");
				}
				return false;
			}
		}

		bool nonterm_b(bool enabled) {
			//output("nonterm_b");
			if ((!isEOI() && (currentChar == '^'))) {
				readChar();
				insertSet(false);
				if (!nonterm_setb(enabled)) return false;
				if (!checkToken(']', "<b>")) return false;
				return true;
			}
			else if (!isEOI() && (currentChar != ']')) {
				insertSet(true);
				if (!nonterm_setb(enabled)) return false;
				if (!checkToken(']', "<b>")) return false;
				return true;
			}
			else if (!isEOI() && (currentChar == ']')) {
				readChar();
				return true;
			}
			else {
				//error 
				if (isEOI()) {
					//errorEnd(currentChar,"<b>","TOKEN_A, TOKEN_C, TOKEN_H, TOKEN_J, TOKEN_O, '|', TOKEN_U");
				}
				else {
					//errorUnExpected(currentChar,"<b>","TOKEN_A, TOKEN_C, TOKEN_H, TOKEN_J, TOKEN_O, '|', TOKEN_U");
				}
				return false;
			}
		}

		bool nonterm_part(bool enabled) {
			//output("nonterm_part");
			//printf("char = '%c'\n",currentChar);
			if ((!isEOI() && (!controlChar()))) {
				insertCharacter(currentChar);
				readChar();
				return true;
			}
			else if (!isEOI() && (currentChar == '\\'))
			{
				readChar();
				insertCharacter(currentChar);
				readChar();
				return true;
			}
			else if (!isEOI() && (currentChar == '.'))
			{
				insertAny();
				readChar();
				return true;
			}
			else if (!isEOI() && (currentChar == '$'))
			{
				insertEnd();
				readChar();
				return true;
			}
			else if (!isEOI() && (currentChar == '^'))
			{
				insertBegin();
				readChar();
				return true;
			}
			else if ((!isEOI() && (currentChar == '('))) {
				
				readChar();
				insertStart<SeqStart>("brace");
				if (!nonterm_s(enabled)) return false;
				if (!checkToken(')', "<part>")) return false;
				insertSeq();
				return true;
			}
			else if ((!isEOI() && (currentChar == '['))) {
				readChar();
				if (!nonterm_b(enabled)) return false;
				return true;
			}
			else {
				//error 
				if (isEOI()) {
					//errorEnd(currentChar,"<part>","TOKEN_A, TOKEN_C, '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '^', TOKEN_U");
				}
				else {
					//errorUnExpected(currentChar,"<part>","TOKEN_A, TOKEN_C, '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '^', TOKEN_U");
				}
				return false;
			}
		}

		bool nonterm_s_sec(bool enabled) {
			//output("nonterm_s_sec");
			while (true)
			{
				if ((!isEOI() && (currentChar == ')')) || (isEOI())) {
					// epsilon
					return true;
				}
				else if ((!isEOI() && (currentChar == '|'))) {
					insertOption();
					readChar();
					if (!nonterm_string(enabled)) return false;
					//if (!nonterm_s_sec(enabled)) return false;
					insertEndOfOption();
					continue;
					return true;
				}
				else {
					//error 
					if (isEOI()) {
						//errorEnd(currentChar,"<s'>","')', '|', end of input");
					}
					else {
						//errorUnExpected(currentChar,"<s'>","')', '|', end of input");
					}
					return false;
				}
			}
		}

		template <bool dummy = false> void backslash_set(char & last)
		{
			readChar();
			if (currentChar == 'n') insertIntoSet<dummy>(last = '\n');
			else if (currentChar == 't') insertIntoSet<dummy>(last = '\t');
			else if (currentChar == 's') insertIntoSet<dummy>(last = ' ');
			else if (currentChar == 'r') insertIntoSet<dummy>(last = '\r');
			else insertIntoSet<dummy>(last = currentChar);
		}

		bool nonterm_setb(bool) {
			//output("nonterm_setb");
			char last{0};
			while (true)
			{
				if ((!isEOI() && (currentChar == ']'))) {
					// epsilon
					return true;
				}
				else if ((!isEOI() && (currentChar == '\\'))) {
					backslash_set(last);
					readChar();
					continue;
					return true;
				}
				else if ((!isEOI() && (currentChar == '-')))
				{
					char first = last;
					readChar();
					if ((!isEOI() && (currentChar == '\\')))
					{
						backslash_set<true>(last);
					}
					else if (!isEOI() && (currentChar == ']'))
					{
						return false;
					}
					else if (!isEOI())
					{
						last = currentChar;
					}
					else return false;
					insertIntoSet2(first,last);
					readChar();
					continue;
				}
				else if (!isEOI()) {
					insertIntoSet(last = currentChar);
					readChar();
					//if (!nonterm_setb(enabled)) return false;
					continue;
					return true;
				}
				else {
					//error 
					if (isEOI()) {
						//errorEnd(currentChar,"<setb>","TOKEN_A, TOKEN_C, TOKEN_H, ']', TOKEN_J, TOKEN_O, TOKEN_U");
					}
					else {
						//errorUnExpected(currentChar,"<setb>","TOKEN_A, TOKEN_C, TOKEN_H, ']', TOKEN_J, TOKEN_O, TOKEN_U");
					}
					return false;
				}
			}
		}

		bool nonterm_string(bool enabled) {
			//printf("nonterm_string: '%c'\n",currentChar);
			if ((!isEOI() && (currentChar == ')')) || (!isEOI() && (currentChar == '|')) || (isEOI())) {
				// epsilon
				return true;
			}
			else if ((!isEOI() && (!controlChar())) || (!isEOI() && (currentChar == '$')) || (!isEOI() && (currentChar == '.')) || (!isEOI() && (currentChar == '\\')) || (!isEOI() && (currentChar == '[')) || (!isEOI() && (currentChar == '(')) || (!isEOI() && (currentChar == '^'))) {
				if (!nonterm_part(enabled)) return false;
				if (!nonterm_a(enabled)) return false;
				return true;
			}
			else {
				//error 
				if (isEOI()) {
					//errorEnd(currentChar,"<string>","TOKEN_A, TOKEN_C, ')', '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '^', '|', TOKEN_U, end of input");
				}
				else {
					//errorUnExpected(currentChar,"<string>","TOKEN_A, TOKEN_C, ')', '$', TOKEN_H, '[', TOKEN_J, TOKEN_O, '(', '^', '|', TOKEN_U, end of input");
				}
				return false;
			}
		}
	};
public:
	static std::string translate(const std::string input, bool ns)
	{
		std::string out;
		
		auto current = input.begin();
		auto end = input.end();
		
		InternalState<char> state(
			[&]()->char{ if (current != end) return *current++; else throw EndOfInput(); }
		);
		
		state.start();
		
		return state.createRegexp(ns);
	}
};

#endif
