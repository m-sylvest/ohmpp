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

template<class T> struct always_false : std::false_type {};

namespace Ohm {
	namespace JSON {
		using json = nlohmann::json;

		void to_json( json &j, AST::StackItem si);

		template< typename T >
		json listp2json( std::list<T*> * );
		
		json to_json( Ohm::AST::StackItem si )
		{
        return std::visit([](auto&& arg) -> json {
					
					using T = std::decay_t<decltype(arg)>;

					Ohm::AST::StackItem s = arg;
					std::cerr << "to_json(), type=" << Ohm::AST::typenames[s.index()] << std::endl;
					
					if constexpr (std::is_same_v<T, AST::name *>)
						return json::object( { "name", arg->s } );

					else if constexpr (std::is_same_v<T, AST::terminal *>)
						return json::object( { "terminal", arg->s } );
#if 1
					// these are currently not used:
					else if constexpr (std::is_same_v<T, AST::caseName *>)
					{
						return json::object( { "caseName", arg ? arg->s : "" } ) ;
					}

					else if constexpr (std::is_same_v<T, AST::RuleDescr *>)
					{
						return json::object( { "ruleDescr", arg ? arg->s : "" } );
					}							

					else if constexpr (std::is_same_v<T, AST::SuperGrammar *>)
					{
						return json::object( { "SuperGrammar", arg ? arg->s : "" } );
					}
#endif
					else if constexpr (std::is_same_v<T, AST::Base *>)
					{
						json parms = listp2json( arg->paramsAlts );
						switch( arg->type )
						{
							case Ohm::AST::Base::Type::Appl:
								return json( {{ "Base", 
										{ { "type", "Appl" }, { "name", arg->name }, { "Params", parms } }
								}} );
								break;

							case Ohm::AST::Base::Type::Term:
								return json( {{ "Base", 
										{ { "type", "Term" }, { "terminal", arg->name } }
								}} );
								break;

							case Ohm::AST::Base::Type::Range:
								return json( {{ "Base", 
										{ { "type", "Range" }, { "from", arg->rangeFrom }, { "to", arg->rangeTo } }
								}} );
								break;

							case Ohm::AST::Base::Type::Alt:
								return json( {{ "Base", 
										{ { "type", "Alt" }, { "Alt", parms } }
								}} );
								break;

							default:	
								assert( false );
								break;
						}
					}

					else if constexpr (std::is_same_v<T, AST::Lex *>)
					{
						json j = to_json(arg->base);
						j["hashPrefix"] = arg->hashPrefix;
						return json( {{ "Lex", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Pred *>)
					{
						json j = to_json(arg->lex);
						j["op"] = arg->op;
						return json( {{ "Pred", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Iter *>)
					{
						json j = to_json(arg->pred);
						j["op"] = arg->op;
						return json( {{ "Iter", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Seq *>)
					{					
						json seq = listp2json<AST::Iter>( arg );
						return json( {{ "Seq", seq }} );
					}							

					else if constexpr (std::is_same_v<T, AST::TopLevelTerm *>)
					{
						json j = listp2json<AST::Iter>( arg->seq );
						return json( {{ "TopLevelTerm", { { "caseName", arg->caseName }, { "Seq", j } } }} );
					}							

					else if constexpr (std::is_same_v<T, AST::ParamsAlt *>)
					{
						json parms = listp2json( arg );
						return json( {{ "Params", parms }} );
					}							

					else if constexpr (std::is_same_v<T, AST::RuleBody *>)
					{
						json body = listp2json( arg );
						return json( {{ "RuleBody", body }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Rule *>)
					{
						std::map<Ohm::AST::Rule::Type,std::string> typeMap = { 
							{ Ohm::AST::Rule::Type::Define, "Define"}, 
							{ Ohm::AST::Rule::Type::Extend, "Extend"}, 
							{ Ohm::AST::Rule::Type::Override, "Override"}
						};
						
						std::string type  = "";
						json parms = listp2json<AST::Seq>( arg->ruleParms );
						json body  = listp2json<AST::TopLevelTerm>( arg->ruleBody );
						
						return json( {{ "Rule", {
							{ "type",			typeMap[arg->type] },
							{ "name",			arg->name },
							{ "RuleDescr",	arg->rulesDescr },
							{ "Params",		parms },
							{ "RuleBody", body }
						}	}} );
					}

					else if constexpr (std::is_same_v<T, AST::Grammar *>)
					{
						json rules = listp2json<AST::Rule>( arg->rules );
						json j = json::object({ 
							{ "name",		arg->name },
							{ "parent",	arg->parent },
							{ "Rules",	rules }
						});
						return json( {{ "Grammar", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Grammars *>)
					{
						json grms = listp2json<AST::Grammar>( arg );
						return json({{ "Grammars", grms }});
					}

					else
						static_assert( always_false<T>::value, "Incomplete visitor :-(" );
        }, si);			
		}

		void to_json( json &j, AST::StackItem si)
		{
			j = to_json( si );
		}
		
		template< typename T >
		json listp2json( std::list<T*> *t )
		{
			json j = json::array();
			if( t )
				for( const auto tt : *t )
					j.emplace_back( to_json(AST::StackItem(tt)) );
			
			return j;
		}
		

	}
}

#endif /* OHM_AST_JSON_HPP */

