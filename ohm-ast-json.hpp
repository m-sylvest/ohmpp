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

//
// http://en.cppreference.com/w/cpp/utility/variant/visit has served as a skeletal example for creating this:
//

namespace Ohm {
	namespace JSON {
		using json = nlohmann::json;

		json to_json( Ohm::AST::StackItem si );

		void to_json( json &j, AST::StackItem si);
		
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

