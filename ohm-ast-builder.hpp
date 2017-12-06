#include "ohm-grammar.hpp"

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <variant>
#include <functional>

namespace Ohm 
{
	// Data structures for building the Abstract Syntax Tree of an Ohm grammar;
	namespace AST 
	{
		using namespace std;
        
		typedef struct Base;

		typedef struct { typedef GRM::Lex GRM; bool hashPrefix; Base* base; } Lex;
		typedef struct { string op; Lex* lex; } Pred;
		typedef struct { Pred* pred; string op; } Iter;
		typedef struct { typedef GRM::Seq GRM; list<Iter *> l; } Seq;
		 
		typedef struct { Seq seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm *> RuleBody;
		typedef struct { string name, rulesDescr, op; list<RuleBody *> ruleBodys; } Rule;

		typedef struct { string name, parent; list<Rule *> rules; } Grammar;
		typedef list<Grammar *> Grammars;

		typedef list<Seq *> ParamsAlt;
		typedef struct Base { 
			enum class Type { Appl, Range, Term, Alt } type;
			string name; 
			ParamsAlt *paramsAlts; 
			string intvFrom, intvTo; 
		} Base;

		typedef struct { string s; } name;
		typedef struct { string s; } terminal;
		typedef struct { string s; } caseName;

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
			ParamsAlt *,
			name *,
			terminal *,
			caseName *
		> StackItem;
	}
};

namespace Ohm {

	namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

	template< typename T >
	static T *pop(std::vector<AST::StackItem> &v)
	{
		if( !v.empty() && std::holds_alternative<T *>(v.back()) )
		{
			std::cerr << "Enter pop(), size=" << v.size() << ", index=" << v.back().index() << std::endl;
			auto e = std::get<T *>(v.back());
			v.pop_back();
			std::cerr << "Exit pop() A, size=" << v.size() << std::endl;
			return e;
		}
		else
		{
			std::cerr << "Exit pop() Z, size=" << v.size() << std::endl;
			return nullptr;
		}
	}
	
	template< typename T >
	static std::list<T *> pop_any(std::vector<AST::StackItem> &v)
	{
		std::cerr << "Enter pop_any, size=" << v.size() << ", index=" << v.back().index() << std::endl;
		std::list<T *> result;

		for( auto top = pop<T>(v); top ; top = pop<T>(v) )
		{
				result.push_back( top );							
		}
		std::cerr << "Exit pop_any, size=" << v.size() << ", result-size=" << result.size() << std::endl;
		std::cerr.flush();
		return result;
	}
	
	template< typename Rule >
	struct action : pegtl::nothing< Rule > 
	{
	};
	
#if 1
	//  Base
	//    = ident Params? ~(ruleDescr? "=" | ":=" | "+=")  -- application
	//    | oneCharTerminal ".." oneCharTerminal           -- range
	//    | terminal                                       -- terminal
	//    | "(" Alt ")"                                    -- paren
	template<> 
	struct action< GRM::Base_Appl >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Base_Appl: "<< in.string() << std::endl;
			auto ps = pop<AST::ParamsAlt>(v);
			v.push_back( new AST::Base{ AST::Base::Type::Appl, pop<AST::name>(v)->s, ps, "", "" } );
		}
	};
#endif
	
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

	template<> 
	struct action< GRM::Seq >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "Seq: "<< in.string() << std::endl;
			
			auto its = pop_any<AST::Iter>(v);
//			v.back() = its;
		}
	};
	
	template<> 
	struct action< GRM::TopLevelTerm >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "TopLevelTerm: "<< in.string() << std::endl;
			
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
			v.push_back( new AST::name{ in.string() } );
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
			v.push_back( new AST::terminal{ s } );
		}
	};

	template<> 
	struct action< GRM::caseName >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "caseName: "<< in.string() << std::endl;
			auto n = pop<AST::name>(v);
			v.push_back( new AST::terminal{ n->s } );
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

	template<>
	struct control< GRM::Seq > : pegtl::normal< GRM::Seq >
	{
		template< typename Input >
		static void start( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cout << "*** Seq, entering, sz=" << v.size() << std::endl;

			v.push_back( static_cast<AST::Seq *>(nullptr) );
		}
		
		template< typename Input >
		static void failure( const Input& in, std::vector<AST::StackItem> &v )
		{
			if( !v.empty() )
				std::cout << "*** Seq, exiting: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
			
			if( auto e = pop<AST::Seq>(v) ; e )
			{
				delete e;
			}
		}
	};

}

