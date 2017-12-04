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
        
		typedef struct { string name; } Base;

		typedef struct { bool hashPrefix; Base* base; } Lex;
		typedef struct { string op; Lex* lex; } Pred;
		typedef struct { Pred* pred; string op; } Iter;
		typedef list<Iter *> Seq;
		typedef struct { Seq seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm *> RuleBody;
		typedef struct { string name, rulesDescr, op; list<RuleBody *> ruleBodys; } Rule;

		typedef struct { string name, parent; list<Rule *> rules; } Grammar;
		typedef list<Grammar *> Grammars;

		typedef list<Seq *> Params;
		typedef list<Seq *> Alt;
		typedef struct Base_Application : Base { string name; list<Params *> params; } Base_Application;
		typedef struct Base_Range :				Base { string intvFrom, intvTo; } Base_Range;
		typedef struct Base_Terminal :		Base { string terminal; }					Base_Terminal;
		typedef struct Base_Parenthesis : Base { list<Alt *> alts; }				Base_Parenthesis;
		
		typedef string name;
		typedef string terminal;

		// The grand unified data structure to hold all values met during compilation:
		typedef variant< 
			Base *,
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

	template< typename T >
	static T *pop(std::vector<AST::StackItem> &v)
	{
		auto e = std::get<T *>(v.back());
		v.pop_back();
		return e;
	}
	
	template< typename T >
	static std::list<T *> pop_any(std::vector<AST::StackItem> &v)
	{
		std::cerr << "Enter pop_any, size=" << v.size() << ", index=" << v.back().index() << std::endl;
		std::list<T *> result;

		if( !v.empty() )
		{
			for( auto top = v.back() ; !v.empty() && std::holds_alternative<T *>(top) ;  )
			{
				result.push_back( pop<T>(v) );				
				if( !v.empty() ) 
					top = v.back();
			}
		}
		std::cerr << "Exit pop_any, size=" << v.size() << ", result-size=" << result.size() << std::endl;
		std::cerr.flush();
		return result;
	}
	
	template< typename Rule >
	struct action : pegtl::nothing< Rule > 
	{
	};

	//  Base
	//    = ident Params? ~(ruleDescr? "=" | ":=" | "+=")  -- application
	//    | oneCharTerminal ".." oneCharTerminal           -- range
	//    | terminal                                       -- terminal
	//    | "(" Alt ")"                                    -- paren
	template<> 
	struct action< GRM::Base >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Base: "<< in.string() << std::endl;
			v.push_back( new AST::Base{ *pop<std::string>(v) } );
		}
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
			std::cout << "LexL: "<< in.string() << std::endl;
			v.push_back( new AST::Lex{ true, pop<AST::Base>(v) } );
		}
	};

	template<> 
	struct action< GRM::Lex_Base >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "LexB: "<< in.string() << std::endl;
			v.push_back( new AST::Lex{ false, pop<AST::Base>(v) } );
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
			v.push_back( new AST::Pred{ "~", pop<AST::Lex>(v) } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lookahead >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Pred_Lookahead: "<< in.string() << std::endl;
			v.push_back( new AST::Pred{ "&", pop<AST::Lex>(v) } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lex >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Pred_Lex: "<< in.string() << std::endl;
			v.push_back( new AST::Pred{ "", pop<AST::Lex>(v) } );
		}
	};

	//  Iter
	//    = Pred "*"  -- star
	//    | Pred "+"  -- plus
	//    | Pred "?"  -- opt
	//    | Pred
	//
	// -- for the sake of stack-based Pred's rewritten to
	//
	//  Iter = Pred ( "*" | "+" | "?" )?
	//
	template<> 
	struct action< GRM::Iter >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			char opc = in.string()[in.string().size()-1];
			std::string op = 
							opc=='*' ? "*" : 
							opc=='+' ? "+" : 
							opc=='?' ? "?" : 
							"";
			std::cout << "Iter: "<< in.string() << std::endl;
			
			v.push_back( new AST::Iter{ pop<AST::Pred>(v), op } );
		}
	};
#if 0
	template<> 
	struct action< GRM::Iter_Star >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_Star: "<< in.string() << std::endl;
			v.push_back( new AST::Iter{ pop<AST::Pred>(v), "*" } );
		}
	};

	template<> 
	struct action< GRM::Iter_Plus >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Iter_Plus: "<< in.string() << std::endl;
			auto *pred = std::get<AST::Pred *>(v.back());
			v.pop_back();
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
			auto *pred = std::get<AST::Pred *>(v.back());
			v.pop_back();
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
			auto *pred = std::get<AST::Pred *>(v.back());
			v.pop_back();
			v.push_back( new AST::Iter{ pred, "" } );
		}
	};
#endif

	template<> 
	struct action< GRM::Seq >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Seq: "<< in.string() << std::endl;
			
			v.push_back( new AST::Seq{ pop_any<AST::Iter>(v) } );
		}
	};
	
	template<> 
	struct action< GRM::name >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "name: "<< in.string() << std::endl;
			v.push_back( new std::string( in.string() ) );
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
			v.push_back( new std::string( s ) );
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
