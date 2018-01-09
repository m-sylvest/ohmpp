/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ohm-ast-builder.hpp"
//
// From https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
//
// Non-copying alternative:
// s.erase(0, s.find_first_not_of(" \n\r\t"));                                                                                               
// s.erase(s.find_last_not_of(" \n\r\t")+1); 
//
std::string trim(const std::string& str,
                 const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

namespace Ohm {

	namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

	void dumpStack( std::vector<AST::StackItem> v )
	{	
		for( size_t i = 0; i<v.size() ; i++ )
		{
			std::cerr << "dS(" << i << ") ~ " << typenameOf(v[i]) << std::endl;
		}
		std::cerr.flush();
	}
	
}
