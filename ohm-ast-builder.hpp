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

		typedef struct { bool hashPrefix; Base* base; } Lex;
		typedef struct { string op; Lex* lex; } Pred;
		typedef struct { Pred* pred; string op; } Iter;
		typedef list<Iter *> Seq;
		 
		typedef struct { Seq *seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm *> RuleBody;
		typedef list<Seq *> ParamsAlt;
		typedef struct { 
			enum class Type { Define, Override, Extend  } type;
			string name, rulesDescr;
			RuleBody *ruleBody;
			ParamsAlt *ruleParms;
		} Rule;

		typedef struct { string name, parent; list<Rule *> *rules; } Grammar;
		typedef list<Grammar *> Grammars;

		typedef struct Base { 
			enum class Type { Appl, Range, Term, Alt } type;
			string name; 
			ParamsAlt *paramsAlts; 
			string rangeFrom, rangeTo; 
		} Base;

		typedef struct { string s; } name;
		typedef struct { string s; } terminal;
		typedef struct { string s; } caseName;
		typedef struct { string s; } SuperGrammar;
		typedef struct { string s; } RuleDescr;

		// The grand unified data structure to hold all values met during compilation:
		typedef variant< 
			Base *,
			Lex *,
			Pred *,
			Iter *,
			Seq *,
			TopLevelTerm *,
			RuleBody *,
			RuleDescr *,
			Rule *,
			Grammar *,
			Grammars *,
			ParamsAlt *,
			name *,
			terminal *,
			caseName *,
			SuperGrammar *
		> StackItem;
	}
};

namespace Ohm {

	namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

	void dumpStack( std::vector<AST::StackItem> v )
	{
		int i = 0;
		for( const auto &e: v )
		{
			std::cerr << "dS(" << i << "), index=" << v[i].index() << std::endl;
			i++;
		}
		std::cerr.flush();
	}
	
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
	static std::list<T *> *pop_any(std::vector<AST::StackItem> &v)
	{
		std::cerr << "Enter pop_any, size=" << v.size() << ", index=" << v.back().index() << std::endl;
		auto result = new std::list<T *>();

		for( auto top = pop<T>(v); top ; top = pop<T>(v) )
		{
				result->push_front( top );							
		}
		std::cerr << "Exit pop_any, size=" << v.size() << ", result-size=" << result->size() << std::endl;
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
	struct action< GRM::Base_Appl >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Base_Appl: "<< in.string() << std::endl;
			auto ps = pop<AST::ParamsAlt>(v);
			v.push_back( new AST::Base{ AST::Base::Type::Appl, pop<AST::name>(v)->s, ps, "", "" } );
		}
	};
	
	template<> 
	struct action< GRM::Base_Terminal >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Base_Terminal: "<< in.string() << std::endl;
			v.push_back( new AST::Base{ AST::Base::Type::Term, pop<AST::name>(v)->s } );
		}
	};
	
	template<> 
	struct action< GRM::Base_Paren >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Base_Paren: "<< in.string() << std::endl;
			v.push_back( new AST::Base{ AST::Base::Type::Alt, "", pop<AST::ParamsAlt>(v) } );
		}
	};
	
	template<> 
	struct action< GRM::Base_Range >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			auto s = in.string();
			const char *from = &s[0], *to = &s[s.size()-1];
			std::cerr << "Base_Range: " << s << " = " << from << ".." << to << std::endl;
			v.push_back( new AST::Base{ AST::Base::Type::Range, "", nullptr, from, to } );
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
			std::cerr << "LexL: "<< in.string() << std::endl;
			v.push_back( new AST::Lex{ true, pop<AST::Base>(v) } );
		}
	};

	template<> 
	struct action< GRM::Lex_Base >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "LexB: "<< in.string() << std::endl;
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
			std::cerr << "Pred_Not: "<< in.string() << std::endl;
			v.push_back( new AST::Pred{ "~", pop<AST::Lex>(v) } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lookahead >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Pred_Lookahead: "<< in.string() << std::endl;
			v.push_back( new AST::Pred{ "&", pop<AST::Lex>(v) } );
		}
	};

	template<> 
	struct action< GRM::Pred_Lex >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Pred_Lex: "<< in.string() << std::endl;
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
			std::cerr << "Iter: "<< in.string() << std::endl;
			
			v.push_back( new AST::Iter{ pop<AST::Pred>(v), op } );
		}
	};

	template<> 
	struct action< GRM::TopLevelTerm >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "TopLevelTerm: "<< in.string() << std::endl;
			
			auto caseName = pop<AST::caseName>(v);
			std::string caseN = caseName ? caseName->s : "";
			v.push_back( new AST::TopLevelTerm{ pop<AST::Seq>(v), caseN } );
		}
	};
	
	//  Rule
	//    = ident Formals? ruleDescr? "="  RuleBody  -- define
	//    | ident Formals?            ":=" RuleBody  -- override
	//    | ident Formals?            "+=" RuleBody  -- extend
	//
	// rewritten to Rule = ident Formals? ( RuleDescr? "=" | ":" | "+" ) ruleBody
	//
	template<> 
	struct action< GRM::Rule >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			auto s = in.string();
			int posEq = s.find('=');
			char op1 = s[posEq-1];
			std::cerr << "Rule: " << s << ", op1=" << op1 << std::endl;

			AST::Rule::Type t = 
							op1=='+' ? AST::Rule::Type::Extend :
							op1==':' ? AST::Rule::Type::Override :
							AST::Rule::Type::Define;

			auto ruleBody = pop<AST::RuleBody>(v);
			auto ruleDescr= pop<AST::RuleDescr>(v);
			auto rdtxt		= ruleDescr ? ruleDescr->s : "";
			auto ruleParms= pop<AST::ParamsAlt>(v);
			auto id				= pop<AST::name>(v);
			v.push_back( new AST::Rule{ t, id->s, rdtxt, ruleBody, ruleParms } );
		}
	};

	template<> 
	struct action< GRM::SuperGrammar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "SuperGrammar: "<< in.string() << std::endl;
			
			v.push_back( new AST::SuperGrammar{ pop<AST::name>(v)->s } );
		}
	};
	
	template<> 
	struct action< GRM::ruleDescrText >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{			
			std::cerr << "RuleDescr: "<< in.string() << std::endl;			
			v.push_back( new AST::RuleDescr{ in.string() } );
		}
	};
	
	template<> 
	struct action< GRM::Grammar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "Grammar: "<< in.string() << std::endl;
			
			auto rules = pop_any<AST::Rule>(v);
			auto super = pop<AST::SuperGrammar>(v);
			auto sgname = super ? super->s : std::string("");
			v.push_back( new AST::Grammar{ pop<AST::name>(v)->s, sgname, rules } );
		}
	};
	
	template<> 
	struct action< GRM::name >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "name: "<< in.string() << std::endl;
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
			std::cerr << "terminal: "<< s << std::endl;
			v.push_back( new AST::terminal{ s } );
		}
	};

	template<> 
	struct action< GRM::caseName >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "caseName: "<< in.string() << std::endl;
			auto n = pop<AST::name>(v);
			v.push_back( new AST::caseName{ n->s } );
		}
	};
#if 0
	template<> 
	struct action< GRM::comment >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "comment: " << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::operator_ >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << in.string() << std::endl;
		}
	};

	template<> 
	struct action< GRM::escapeChar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<AST::StackItem> &v )
		{
			auto s = in.string();
			std::cerr << "escapeChar(" << s.size() << ")" << std::endl;
			for (auto i = 0; i < s.length(); ++i)
				std::cerr << std::setw(2) << std::setfill('0') << std::hex << (int)s[i];

			std::cerr << std::endl;
		}
	};
#endif
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
			std::cerr << "*** Seq, start, sz=" << v.size() << std::endl;

			v.push_back( static_cast<AST::Seq *>(nullptr) );
		}
		
		template< typename Input >
		static void failure( const Input& in, std::vector<AST::StackItem> &v )
		{
			if( !v.empty() )
				std::cerr << "*** Seq, failure: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
			
			if( auto e = pop<AST::Seq>(v) ; e )
			{
				delete e;
			}
		}

		template< typename Input >
		static void success( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "*** Seq, success, sz=" << v.size() << std::endl;
			dumpStack(v);
			
			v.back() = pop_any<AST::Iter>(v);
			std::cerr << "Seq: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
		}
	};

	template<>
	struct control< GRM::RuleBody > : pegtl::normal< GRM::RuleBody >
	{
		template< typename Input >
		static void start( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "*** RuleBody, start, sz=" << v.size() << std::endl;

			v.push_back( static_cast<AST::RuleBody *>(nullptr) );
		}
		
		template< typename Input >
		static void failure( const Input& in, std::vector<AST::StackItem> &v )
		{
			if( !v.empty() )
				std::cerr << "*** RuleBody, failure: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
			
			if( auto e = pop<AST::RuleBody>(v) ; e )
			{
				delete e;
			}
		}

		template< typename Input >
		static void success( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "*** RuleBody, success, sz=" << v.size() << std::endl;
			dumpStack(v);
			
			v.back() = pop_any<AST::TopLevelTerm>(v);
			std::cerr << "RuleBody: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
		}
	};
	
	template<>
	struct control< GRM::Grammars > : pegtl::normal< GRM::Grammars >
	{
		template< typename Input >
		static void start( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "*** Grammars, start, sz=" << v.size() << std::endl;

			v.push_back( static_cast<AST::Grammars *>(nullptr) );
		}
		
		template< typename Input >
		static void failure( const Input& in, std::vector<AST::StackItem> &v )
		{
			if( !v.empty() )
				std::cerr << "*** Grammars, failure: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
			
			if( auto e = pop<AST::Grammars>(v) ; e )
			{
				delete e;
			}
		}

		template< typename Input >
		static void success( const Input& in, std::vector<AST::StackItem> &v )
		{
			std::cerr << "*** Grammars, success, sz=" << v.size() << std::endl;
			dumpStack(v);
			
			v.back() = pop_any<AST::Grammar>(v);
			std::cerr << "Grammars: sz=" << v.size() << ", index=" << v.back().index() << std::endl;
		}
	};
	
}
