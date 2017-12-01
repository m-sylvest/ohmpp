#include <string>
#include <list>

namespace Ohm 
{
	namespace data 
{
		using namespace std;

		class Base;

		typedef struct { char op; Base *base; } Lex;
		typedef struct { char op; Lex lex; } Pred;
		typedef struct { Pred pred; char op; } Iter;
		typedef list<Iter> Seq;
		typedef list<Seq> Params;
		typedef list<Seq> Alt;
		typedef struct { Seq seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm> RuleBody;
		typedef struct { string name, rulesDescr, op; list<RuleBody> ruleBodys; } Rule;

		typedef struct { string name, parent; list<Rule> rules; } Grammar;
		typedef list<Grammar> Grammars;

		class Base { 
		public:
			string name; list<Params> params; string ruleDescr, op;  
			string intvFrom, intvTo;
			string terminal; 
			list<Alt> alts; 
		};

		typedef string escapeChar;
	}
};

#include "ohm-grammar.cpp"


