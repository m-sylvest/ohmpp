/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ohm-ast-json.hpp
 * Author: mikael
 *
 * Created on December 11, 2017, 6:12 AM
 */

#ifndef OHM_AST_JSON_HPP
#define OHM_AST_JSON_HPP

#include "ohm-ast.hpp"
#include <json/json.hpp>


namespace Ohm {
	namespace JSON {
		using json = nlohmann::json;

//		void to_json( json &j, Ohm::AST::Base &b);
		
		json to_json( json &j, Ohm::AST::StackItem si )
		{
        return std::visit([&j](auto&& arg) -> json {
					
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::is_same_v<T, AST::name>)
						return json( { "name", *std::get<0>(arg)->s } );

					else if constexpr (std::is_same_v<T, AST::terminal>)
						return json( { "terminal", *std::get<1>(arg)->s } );

					else if constexpr (std::is_same_v<T, AST::caseName>)
					{
						auto se = std::get<2>(arg);
						return json( { "caseName", se ? se->s : "" } ) ;
					}

					else if constexpr (std::is_same_v<T, AST::RuleDescr>)
					{
						auto se = std::get<3>(arg);
						return json( { "ruleDescr", se ? se->s : "" } );
					}							

					else if constexpr (std::is_same_v<T, AST::SuperGrammar>)
					{
						auto se = std::get<4>(arg);
						return json( { "SuperGrammar", se ? se->s : "" } );
					}

					else if constexpr (std::is_same_v<T, AST::Base>)
					{
						switch( auto b = std::get<5>(arg) ; b->type )
						{
							case Ohm::AST::Base::Type::Appl:
								return json( { "Base", 
										{ { "type", "Appl" }, { "name", b->name }, { "Params", to_json(b->ParamsAlt) } }
								} );
								break;

							case Ohm::AST::Base::Type::Term:
								return json( { "Base", 
										{ { "type", "Term" }, { "terminal", b->name } }
								} );
								break;

							case Ohm::AST::Base::Type::Range:
								return json( { "Base", 
										{ { "type", "Range" }, { "from", b->rangeFrom }, { "to", b->rangeTo } }
								} );
								break;

							case Ohm::AST::Base::Type::Alt:
								return json( { "Base", 
										{ { "type", "Alt" }, { "Alt", to_json(b->ParamsAlt) } }
								} );
								break;

							default:	
								break;
						}
					}

					else if constexpr (std::is_same_v<T, AST::Lex>)
					{
						auto lex = std::get<6>(arg);
						return json( { "Lex", 
								{ { "hashTagged", lex->hashTagged }, { to_json(lex->Base) } }
						} );
					}							

					else if constexpr (std::is_same_v<T, AST::Pred>)
					{
						auto pred = std::get<7>(arg);
						return json( { "Pred", 
								{ { "op", pred->op }, { to_json(pred->Lex) } }
						} );
					}							

					else if constexpr (std::is_same_v<T, AST::Iter>)
					{
						auto iter = std::get<8>(arg);
						return json( { "Iter", 
								{ { "op", iter->op }, { to_json(iter->Pred) } }
						} );
					}							

					else if constexpr (std::is_same_v<T, AST::Seq>)
					{
						auto seq = std::get<9>(arg);
						return json( { "Seq", j_list(seq->Iter) } );
					}							

					else if constexpr (std::is_same_v<T, AST::TopLevelTerm>)
					{
						auto top = std::get<10>(arg);
						return json( { "TopLevelTerm", { "caseName", top->caseName ? top->caseName : "" }, to_json(top->Seq) } );
					}							

					else if constexpr (std::is_same_v<T, AST::ParamsAlt>)
					{
						auto parms = std::get<11>(arg);
						return json( { "Params", parms ? j_list(parms) : json() } );
					}							

					else if constexpr (std::is_same_v<T, AST::RuleBody>)
					{
						auto rbody = std::get<12>(arg);
						return json( { "RuleBody", j_list(rbody) } );
					}							

					else if constexpr (std::is_same_v<T, AST::Rule>)
					{
						auto rule = std::get<13>(arg);
						std::map<Ohm::AST::Rule::Type,std::string> typeMap = { 
							{ Ohm::AST::Rule::Type::Define, "Define"}, 
							{ Ohm::AST::Rule::Type::Extend, "Extend"}, 
							{ Ohm::AST::Rule::Type::Override, "Override"}
						};
						return json( { "Rule", 
							{ "type",			typeMap[rule->type] },
							{ "name",			rule->name },
							{ "Params",		to_json(rule->ParamsAlt) },
							{ "RuleBody", to_json(rule->ruleBody) }
						} );
					}							

					else if constexpr (std::is_same_v<T, AST::Grammar>)
					{
						auto gram = std::get<14>(arg);
						return json( { "Grammar", 
							{ "name",		gram->name },
							{ "parent",	gram->parent },
							{ "Rules",	j_list(gram->rules) }
						} );
					}							

					else if constexpr (std::is_same_v<T, AST::Grammars>)
					{
						auto se = std::get<15>(arg);
						return json( { "Grammars", to_json(se) } );
					}
#if 0
					else
						static_assert( false, "Incomplete visitor :-(" );
#endif
        }, si);			
		}
		
	}
}

#endif /* OHM_AST_JSON_HPP */

