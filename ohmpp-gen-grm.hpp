/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ohmpp-gen-grm.hpp
 * Author: mikael
 *
 * Created on December 13, 2017, 11:15 PM
 */

#ifndef OHMPP_GEN_GRM_HPP

#define OHMPP_GEN_GRM_HPP

#include "ohm-ast.hpp"
#include <map>



template < typename T, typename U = std::string, typename S = std::string >
U join( T cont, S sep )
{
	auto b = cont.cbegin();
	auto e = cont.cend();
	
	U result;
	if( b != e )
		result += *b++;
	
	while( b != e )
	{
		result += sep;
		result += *b++;
	}
	return result;
}

template < typename T, typename F >
std::string map_join( std::list<T> *l, F mapper, std::string sep )
{
	std::list<std::string> result;
	
	if( l ) 
		std::transform(l->cbegin(), l->cend(), std::back_inserter(result), mapper);
	
	return join( result, sep );
}

#if 0
std::string to_string2( int i )
{
	return "x";
}

std::string test_map_join()
{
	std::list<int> v = { 1,2,3 };
	std::function f = to_string2 ;
	return map_join( &v, f, " " );	
}
#endif

std::string replace( const std::string template_, const std::map<std::string,std::string> replacements )
{
	std::string result = template_;
	for( const auto [k,v] : replacements )
	{
		std::string repl_key = std::string("{{") + k +  std::string("}}") ;
		result.replace( result.find(repl_key), repl_key.length(), v);
	}
	return result;
}

namespace Ohm {
	namespace GenGRM {
#if 0
		std::string to_pegtlIter( AST::Iter *r )
		{
			if( r->op == "*" ) 
			{
				
				
			}
			else if( r->op == "+" ) 
			{
				
			}
			else
			{
				
			};			
		}
	
		std::string to_pegtlTopLevelTerm( AST::TopLevelTerm *r )
		{
			return "";			
		}
	
		std::string to_pegtlGrammar( AST::Grammar *g )
		{
			const std::string GrammarTemplate = "struct {{GRAMMAR}}_ : {{SEQ}}<{{RULES}}> {};";

			std::map<std::string,std::string> repl = { 
				{ "GRAMMAR",	g->name }, 
				{ "SEQ",			(g->name[0] & 0x20) ? "seq" :"SEQ" },
				{ "RULES",		map_join( g->rules, to_pegtlRule, std::string(", ") )  }
			};
			
			return replace( GrammarTemplate, repl );			
		}
#endif
		
		std::string to_pegtl( Ohm::AST::StackItem si )
		{
        return std::visit([](auto&& arg) -> std::string {
					
					using T = std::decay_t<decltype(arg)>;

					Ohm::AST::StackItem s = arg;
					std::cerr << "to_pegtl(), type=" << Ohm::AST::typenames[s.index()] << std::endl;
					
					if constexpr (std::is_same_v<T, AST::name *>)
						return arg->s;

					else if constexpr (std::is_same_v<T, AST::terminal *>)
						return arg->s;
#if 1
					// these are currently not used:
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
							case Ohm::AST::Base::Type::Appl:
								return arg->name;
								break;

							case Ohm::AST::Base::Type::Term:
							{
								std::list<std::string> as_letters;
								
								std::transform( arg->name.cbegin(), arg->name.cend(), std::back_inserter(as_letters), [](char c){ return std::string("'") + c + "'";  } );
								
								return "string< " + join( as_letters, "," ) + " >";
								break;
							}
							case Ohm::AST::Base::Type::Range:
								return "range< arg->rangeFrom[0], arg->rangeto[0] >";
								break;

							case Ohm::AST::Base::Type::Alt:
								return "SOR< " + to_pegtl(arg->paramsAlts) + " >";
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
							return "hash< " + to_pegtl( arg->base ) + " >";	// TODO
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
							return "not_at< " + to_pegtl( arg->lex ) + " >";
						}
						else if( arg->op == "&" ) 
						{
							return "at< " + to_pegtl( arg->lex ) + " >";
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
							return "star< " + to_pegtl( arg->pred ) + " >";
						}
						else if( arg->op == "+" ) 
						{
							return "plus< " + to_pegtl( arg->pred ) + " >";
						}
						else if( arg->op == "?" ) 
						{
							return "opt< " + to_pegtl( arg->pred ) + " >";
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
						return "SEQ< " + map_join(arg->seq, to_pegtl, ", " ) + " >";
					}							

					else if constexpr (std::is_same_v<T, AST::ParamsAlt *>)
					{
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::RuleBody *>)
					{
						return "";
					}							

					else if constexpr (std::is_same_v<T, AST::Rule *>)
					{
						const std::string RuleTemplate = "struct {{RULE}}_ : {{SOR}}<{{BODY}}> {};";

						std::map<std::string,std::string> repl = { 
							{ "RULE",			arg->name }, 
							{ "SOR",			(arg->name[0] & 0x20) ? "sor" :"SOR" },
							{ "BODY",			map_join( arg->ruleBody, to_pegtl, ", " )  }
						};

						return replace( RuleTemplate, repl );			
					}

					else if constexpr (std::is_same_v<T, AST::Grammar *>)
					{
						return " struct " + arg->name + " : " + map_join( arg->rules, to_pegtl, ";\n\n" ) + " };"; 
					}							

					else if constexpr (std::is_same_v<T, AST::Grammars *>)
					{
						return map_join( arg, to_pegtl, std::string("\n\n\n") );
					}

					else
            static_assert(always_false<T>::value, "non-exhaustive visitor!");

        }, si);			
		}
#if 0
		template< typename T >
		std::string listp2pegtl( std::list<T*> *t )
		{
			std::list<std::string> l;
			if( t )
				for( const auto tt : *t )
					l.emplace_back( to_pegtl(AST::StackItem(tt)) );
			
			return l;
		}
#endif
	}
}
#endif /* OHMPP_GEN_GRM_HPP */

