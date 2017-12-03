#include "ohm-grammar.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <variant>

namespace Ohm 
{
	// Data structures for building the Abstract Syntax Tree of an Ohm grammar;
	namespace AST 
	{
		using namespace std;
        
		typedef struct {} Base;

		typedef struct { bool hashPrefix; Base* base; } Lex;
		typedef struct { string op; Lex* lex; } Pred;
		typedef struct { Pred* pred; string op; } Iter;
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
		
		typedef string name;
		typedef string terminal;

		// The grand unified data structure to hold all values met during compilation:
		typedef variant< 
			Lex *,
			Pred *,
			Iter *,
			Seq *,
			TopLevelTerm *,
			RuleBody *,
			Rule *,
			Grammar *,
			Grammars *,
			Params *,
			Alt *,
			Base_Application *,
			Base_Range *,
			Base_Terminal *,
			Base_Parenthesis *,
			string *
		> StackItem;
	}
};

namespace Ohm {

	namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

	template< typename Rule >
	struct action : pegtl::nothing< Rule > 
	{
	};

	//  Lex
	//    = "#" Base  -- lex
	//    | Base
	template<> 
	struct action< GRM::Lex_Lex >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Lex: "<< in.string() << std::endl;
			AST::Base *base = v.pop_back();
			v.push_back( new AST::Lex{ true, base } );
		}
	};

	template<> 
	struct action< GRM::Lex_Base >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Lex: "<< in.string() << std::endl;
			AST::Base *base = v.pop_back();
			v.push_back( new AST::Lex{ false, base } );
		}
	};

	//  Pred
	//    = "~" Lex  -- not
	//    | "&" Lex  -- lookahead
	//    | Lex
	template<> 
	struct action< GRM::Pred_Not >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Pred_Not: "<< in.string() << std::endl;
			AST::Lex *lex = v.pop_back();
			v.push_back( new AST::Pred{ "~", lex } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lookahead >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Pred_Lookahead: "<< in.string() << std::endl;
			AST::Lex *lex = v.pop_back();
			v.push_back( new AST::Pred{ "&", lex } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lex >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Pred_Lex: "<< in.string() << std::endl;
			AST::Lex *lex = v.pop_back();
			v.push_back( new AST::Pred{ "", lex } );
		}
	};

	//  Iter
	//    = Pred "*"  -- star
	//    | Pred "+"  -- plus
	//    | Pred "?"  -- opt
	//    | Pred
	template<> 
	struct action< GRM::Iter_Star >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_Star: "<< in.string() << std::endl;
			AST::Pred *pred = v.pop_back();
			v.push_back( new AST::Iter{ pred, "*" } );
		}
	};

	template<> 
	struct action< GRM::Iter_Plus >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_Plus: "<< in.string() << std::endl;
			AST::Pred *pred = v.pop_back();
			v.push_back( new AST::Iter{ pred, "+" } );
		}
	};

	template<> 
	struct action< GRM::Iter_Opt >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_Opt: "<< in.string() << std::endl;
			AST::Pred *pred = v.pop_back();
			v.push_back( new AST::Iter{ pred, "?" } );
		}
	};

	template<> 
	struct action< GRM::Iter_Pred >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_: "<< in.string() << std::endl;
			AST::Pred *pred = v.pop_back();
			v.push_back( new AST::Iter{ pred, "" } );
		}
	};

	
	template<> 
	struct action< GRM::name >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "name: "<< in.string() << std::endl;
			v.push_back( new AST::string( in.string() ) );
		}
	};

	template<> 
	struct action< GRM::terminal >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			// strip leading and trailing '"':
			int sz = in.string().size();
			std::string s = in.string().substr(1,sz-2);
			std::cout << "terminal: "<< s << std::endl;
			v.push_back( new AST::string( s ) );
		}
	};

	template<> 
	struct action< GRM::caseName >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "caseName: "<< in.string() << std::endl;
			// silently pass input 'name' as 'caseName' as StackItem
		}
	};

	template<> 
	struct action< GRM::comment >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "comment: " << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::operator_ >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::escapeChar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
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
