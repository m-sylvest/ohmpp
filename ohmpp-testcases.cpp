/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ohm-ast-builder.hpp"
#include "ohm-gen-grm.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

#include <catch/catch.hpp>

namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

TEST_CASE( "Test a simple grammar", "[GRM][AST]" )
{
  SECTION( "A very simple grammar" )
  {
    char *text = (char *) R"(
navne { 
  Peter += Jakob* "jesper" Tina+ -- karl

  Jakob = "jakob" -- ost
  Tina  = "TINA" -- x
}
)";
    pegtl::argv_input<> in( &text, 0 );
    std::vector<Ohm::AST::StackItem> v;
    
    REQUIRE( pegtl::parse< 
      pegtl::must< 
          Ohm::GRM::Grammars
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v ) == true );
  }
  SECTION( "Another simple grammar" )
  {
    REQUIRE( 1 == 1 );
  }
}
