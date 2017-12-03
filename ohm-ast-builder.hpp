#include "ohm-grammar.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>

namespace Ohm 
{
	// Data structures for building the Abstract Syntax Tree of an Ohm grammar;
	namespace AST 
	{
		using namespace std;
        
		typedef struct {} Base;

		typedef struct { bool hashPrefix; Base& base; } Lex;
		typedef struct { string op; Lex& lex; } Pred;
		typedef struct { Pred& pred; string op; } Iter;
		typedef list<Iter> Seq;
		typedef struct { Seq seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm> RuleBody;
		typedef struct { string name, rulesDescr, op; list<RuleBody> ruleBodys; } Rule;

		typedef struct { string name, parent; list<Rule> rules; } Grammar;
		typedef list<Grammar> Grammars;

		typedef list<Seq> Params;
		typedef list<Seq> Alt;
		typedef struct Base_Application : Base { string name; list<Params> params; } Base_Application;
		typedef struct Base_Range :				Base { string intvFrom, intvTo; } Base_Range;
		typedef struct Base_Terminal :		Base { string terminal; }					Base_Terminal;
		typedef struct Base_Parenthesis : Base { list<Alt> alts; }					Base_Parenthesis;

		typedef string escapeChar;
	}
};

namespace Ohm {

	namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

	template< typename Rule >
	struct action : pegtl::nothing< Rule > 
	{
	};

	template<> 
	struct action< GRM::name >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << "name: "<< in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::terminal >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			int s = in.string().size();
			std::cout << "terminal: "<< in.string().substr(1,s-2) << std::endl;
		}
	};

	template<> 
	struct action< GRM::caseName >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << "caseName: "<< in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::comment >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << "comment: " << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::operator_ >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::escapeChar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			auto s = in.string();
			std::cout << "escapeChar(" << s.size() << ")" << std::endl;
			for (auto i = 0; i < s.length(); ++i)
				std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)s[i];

			std::cout << std::endl;
		}
	};

	template< typename Rule >
	struct control : pegtl::normal< Rule >
	{
	};

}
