/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ohm-gen-grm.hpp"

std::string replace( const std::string &template_, const std::map<std::string,std::string> &replacements )
{
	std::string result = template_;
	for( const auto [k,v] : replacements )
	{
		std::string repl_key = "{{" + k + "}}";
		result.replace( result.find(repl_key), repl_key.length(), v);
	}
	return result;
}

std::string templ( std::string t, std::string s )
{
	return t + "< " + s + " >";
}

std::string get_forward_decl( Ohm::AST::Rule *r )
{
	return "  struct " + r->name + "_;";
}

namespace Ohm {
	namespace GenGRM {
		
		std::string to_pegtl( AST::StackItem si )
		{
        return std::visit([](auto&& arg) -> std::string {
					
					using T = std::decay_t<decltype(arg)>;

					std::cerr << "to_pegtl(), type=" << AST::typenameOf(arg) << std::endl;
					
					if constexpr (std::is_same_v<T, AST::name *>)
						return arg->s + "_";

					else if constexpr (std::is_same_v<T, AST::terminal *>)
						return arg->s;
#if 1
					// these 3 symbols are currently copied by ast-builder and thus not JSON/PEGTL generated:
					else if constexpr (std::is_same_v<T, AST::caseName *>)
					{
						return arg ? arg->s : "";
					}

					else if constexpr (std::is_same_v<T, AST::RuleDescr *>)
					{
						return arg ? arg->s : "";
					}							

					else if constexpr (std::is_same_v<T, AST::SuperGrammar *>)
					{
						return arg ? arg->s : "";
					}
#endif
					else if constexpr (std::is_same_v<T, AST::Base *>)
					{
						switch( arg->type )
						{
							case AST::Base::Type::Appl:
								return arg->name + "_";
								break;

							case AST::Base::Type::Term:
							{
								std::list<std::string> as_letters;
								
								std::transform( arg->name.cbegin(), arg->name.cend(), std::back_inserter(as_letters), [](char c){ return std::string("'") + c + "'";  } );
								
								return templ( "string", join(as_letters, ",") );
								break;
							}
							case AST::Base::Type::Range:
							{
								std::list<std::string> intv = { arg->rangeFrom.substr(0,1), arg->rangeTo.substr(0,1) };
								return templ( "range", join(intv, ",") );
								break;
							}
							case AST::Base::Type::Alt:
								return templ( "SOR", to_pegtl(arg->alts) );
								break;

							default:	
								std::cerr << "Unhandled case value, " << static_cast<int>(arg->type) << std::endl;
								assert( false );
								break;
						}
					}

					else if constexpr (std::is_same_v<T, AST::Lex *>)
					{
						if( arg->hashPrefix ) 
						{
							return templ( "hash", to_pegtl( arg->base ) );	// TODO
						}
						else
						{
							return to_pegtl( arg->base );
						};			
					}

					else if constexpr (std::is_same_v<T, AST::Pred *>)
					{
						if( arg->op == "~" ) 
						{
							return templ( "not_at",to_pegtl( arg->lex ) );
						}
						else if( arg->op == "&" ) 
						{
							return templ( "at", to_pegtl( arg->lex ) );
						}
						else
						{
							return to_pegtl( arg->lex );
						};			
					}							

					else if constexpr (std::is_same_v<T, AST::Iter *>)
					{
						if( arg->op == "*" ) 
						{
							return templ( "star", to_pegtl( arg->pred ) );
						}
						else if( arg->op == "+" ) 
						{
							return templ( "plus", to_pegtl( arg->pred ) );
						}
						else if( arg->op == "?" ) 
						{
							return templ( "opt",  to_pegtl( arg->pred ) );
						}
						else
						{
							return to_pegtl( arg->pred );
						};			
					}							

					else if constexpr (std::is_same_v<T, AST::Seq *>)
					{					
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::TopLevelTerm *>)
					{
						return templ( "SEQ", map_join(arg->seq, to_pegtl, ", " ) );
					}							

					else if constexpr (std::is_same_v<T, AST::Params *>)
					{
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::Alt *>)
					{
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::RuleBody *>)
					{
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::Rule *>)
					{
						const std::string RuleTemplate = "  struct {{RULE}}_ : {{SOR}}<{{BODY}}> {};";

						std::map<std::string,std::string> repl = { 
							{ "RULE",			arg->name }, 
							{ "SOR",			(arg->name[0] & 0x20) ? "sor" :"SOR" },
							{ "BODY",			map_join( arg->ruleBody, to_pegtl, ", " )  }
						};

						return replace( RuleTemplate, repl );			
					}

					else if constexpr (std::is_same_v<T, AST::Grammar *>)
					{
						auto pegtl_template = R"(
namespace {{GRM_NAME}} {

  using namespace tao::TAOCPP_PEGTL_NAMESPACE;
  using namespace Ohm::GRM;

{{FWD_DECLS}}

{{PEGTL_DECLS}} 	

  struct start__symbol : {{FIRST_RULE}} {};
}
)";
						AST::Rule  *firstRule = (arg->rules && arg->rules->size()>0) ? *(arg->rules->begin()) : nullptr;
						std::string firstRuleName = firstRule ? firstRule->name + "_" : "success";

						std::map<std::string,std::string> repl = { 
							{ "GRM_NAME",			arg->name }, 
							{ "FWD_DECLS",		map_join( arg->rules, get_forward_decl, "\n" ) },
							{ "PEGTL_DECLS",	map_join( arg->rules, to_pegtl, "\n" ) },
							{ "FIRST_RULE",		firstRuleName }
						};
						
						return replace( pegtl_template, repl );
					}

					else if constexpr (std::is_same_v<T, AST::Grammars *>)
					{
						return "\n#include <ohmpp.hpp>\n\n" + map_join( arg, to_pegtl, "\n\n" );
					}

					else
            static_assert(always_false<T>::value, "non-exhaustive visitor!");

        }, si);			
		}

	}
}

