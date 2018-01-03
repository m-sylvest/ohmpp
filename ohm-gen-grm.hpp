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
#include <functional>
#include <iostream>
#include <assert.h>

template<class T> struct always_false : std::false_type {};

//
// This will hold the namespace prefix of the grammar, such as 'myGrammar::'
//
const std::string NP;

template < typename T, typename U = std::string, typename S = std::string >
U join( T &cont, S &sep )
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

std::string replace( const std::string &template_, const std::map<std::string,std::string> &replacements );

std::string templ( std::string t, std::string s );

std::string get_forward_decl( Ohm::AST::Rule *r );


namespace Ohm {
	namespace GenGRM {
		
		std::string to_pegtl( AST::StackItem si );
	}
}
#endif /* OHMPP_GEN_GRM_HPP */

