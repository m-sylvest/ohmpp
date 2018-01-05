/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ohm-ast-json.hpp"
#include <iostream>

//
// http://en.cppreference.com/w/cpp/utility/variant/visit has served as a skeletal example for creating this:
//

template<class T> struct always_false : std::false_type {};

namespace Ohm {
	namespace JSON {
		using json = nlohmann::json;

		void to_json( json &j, AST::StackItem si);

		json to_json( Ohm::AST::StackItem si )
		{
        return std::visit([](auto&& arg) -> json {
					
					using T = std::decay_t<decltype(arg)>;

					Ohm::AST::StackItem s = arg;
					std::cerr << "to_json(), type=" << Ohm::AST::typenames[s.index()] << std::endl;
					
					if constexpr (std::is_same_v<T, AST::name *>)
						return json( {{ "name", arg->s }} );

					else if constexpr (std::is_same_v<T, AST::terminal *>)
						return json( {{ "terminal", arg->s }} );
#if 1
					// these are currently not used:
					else if constexpr (std::is_same_v<T, AST::caseName *>)
					{
						return json( {{ "caseName", arg ? arg->s : "" }} ) ;
					}

					else if constexpr (std::is_same_v<T, AST::RuleDescr *>)
					{
						return json( {{ "ruleDescr", arg ? arg->s : "" }} );
					}							

					else if constexpr (std::is_same_v<T, AST::SuperGrammar *>)
					{
						return json( {{ "SuperGrammar", arg ? arg->s : "" }} );
					}
#endif
					else if constexpr (std::is_same_v<T, AST::Base *>)
					{
						switch( arg->type )
						{
							case Ohm::AST::Base::Type::Appl:
								return json( {{ "Base", { 
									{ "type", "Appl" }, 
									{ "name", arg->name }, 
									{ "Params", listp2json( arg->paramsAlts ) } 
								} }} );
								break;

							case Ohm::AST::Base::Type::Term:
								return json( {{ "Base", { 
									{ "type", "Term" }, 
									{ "terminal", arg->name } 
								} }} );
								break;

							case Ohm::AST::Base::Type::Range:
								return json( {{ "Base", { 
									{ "type", "Range" }, 
									{ "from", arg->rangeFrom }, 
									{ "to", arg->rangeTo } 
								} }} );
								break;

							case Ohm::AST::Base::Type::Alt:
								return json( {{ "Base",  { 
									{ "type", "Alt" }, 
									{ "Alt", listp2json( arg->paramsAlts ) } 
								} }} );
								break;

							default:	
								std::cerr << "Unhandled case value, " << static_cast<int>(arg->type) << std::endl;
								assert( false );
								break;
						}
					}

					else if constexpr (std::is_same_v<T, AST::Lex *>)
					{
						json &&j = to_json(arg->base);
						j["hashPrefix"] = arg->hashPrefix;
						return json( {{ "Lex", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Pred *>)
					{
						json &&j = to_json(arg->lex);
						j["op"] = arg->op;
						return json( {{ "Pred", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Iter *>)
					{
						json &&j = to_json(arg->pred);
						j["op"] = arg->op;
						return json( {{ "Iter", j }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Seq *>)
					{					
						return json( {{ "Seq", listp2json( arg ) }} );
					}							

					else if constexpr (std::is_same_v<T, AST::TopLevelTerm *>)
					{
						return json( {{ "TopLevelTerm", { 
							{ "caseName", arg->caseName }, 
							{ "Seq", listp2json( arg->seq ) } 
						} }} );
					}							

					else if constexpr (std::is_same_v<T, AST::ParamsAlt *>)
					{
						return json( {{ "Params", listp2json( arg ) }} );
					}							

					else if constexpr (std::is_same_v<T, AST::RuleBody *>)
					{
						return json( {{ "RuleBody", listp2json( arg ) }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Rule *>)
					{
						std::map<Ohm::AST::Rule::Type,std::string> typeMap = { 
							{ Ohm::AST::Rule::Type::Define, "Define"}, 
							{ Ohm::AST::Rule::Type::Extend, "Extend"}, 
							{ Ohm::AST::Rule::Type::Override, "Override"}
						};
						
						return json( {{ "Rule", {
							{ "type",			typeMap[arg->type] },
							{ "name",			arg->name },
							{ "RuleDescr",	arg->rulesDescr },
							{ "Params",		listp2json( arg->ruleParms ) },
							{ "RuleBody", listp2json( arg->ruleBody ) }
						}	}} );
					}

					else if constexpr (std::is_same_v<T, AST::Grammar *>)
					{
						return json( {{ "Grammar", { 
							{ "name",		arg->name },
							{ "parent",	arg->parent },
							{ "Rules",	listp2json( arg->rules ) }
						} }} );
					}							

					else if constexpr (std::is_same_v<T, AST::Grammars *>)
					{
						return json({{ "Grammars", listp2json( arg ) }});
					}

					else
            static_assert(always_false<T>::value, "non-exhaustive visitor!");

        }, si);			
		}

		void to_json( json &j, AST::StackItem si)
		{
			j = to_json( si );
		}

	}
}
