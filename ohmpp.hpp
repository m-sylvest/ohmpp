/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ohmpp.hpp
 * Author: mikael
 *
 * Created on December 18, 2017, 9:14 AM
 */

#ifndef OHMPP_HPP
#define OHMPP_HPP

#include <pegtl.hpp>

namespace Ohm {

  namespace GRM {

    using namespace tao::TAOCPP_PEGTL_NAMESPACE;

		struct space_;
		
    // helpers for Ohm - capitalisation / padding;
    template < typename ...T >
    struct STAR : pad< star<T ...>, space_ > {};

    template < typename ... T >
    struct SEQ : pad< seq<T ...>, space_ > {};

    template < typename ... T >
    struct SOR : pad< sor<T ...>, space_ > {};

    template < typename ... T >
    struct OPT : pad< opt<T ...>, space_ > {};

    // NonEmptyListOf:
    template < typename T, typename P >
    struct NonEmptyListOf : pad< list< T, P >, space_ > {};

    // ListOf:
    template < typename T, typename P >
    struct ListOf : opt< NonEmptyListOf<T,P> > {};
	}
}

#endif /* OHMPP_HPP */
