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
#if 1
  SECTION( "A very simple grammar" )
  {
    char *text = (char *) R"(
navne { 
  Peter = Jakob* "jesper" Tina+ -- karl

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
#endif  

#if 1  
  SECTION( "RuleBody of first line of CSV example from https://github.com/harc/ohm/blob/master/examples/csv/csv.ohm" )
  {
    char *text = (char *) R"( row1 (eol1 ~end1 row1)* eol1 )";
    
    pegtl::argv_input<> in( &text, 0 );
    std::vector<Ohm::AST::StackItem> v;
    
    REQUIRE( pegtl::parse< 
      pegtl::must< 
          Ohm::GRM::RuleBody
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v ) == true );
  }
#endif  

#if 1  
  SECTION( "Rule of first line of CSV example from https://github.com/harc/ohm/blob/master/examples/csv/csv.ohm" )
  {
    char *text = (char *) R"( csv2 = row2 (eol2 ~end row2)* eol2 )";
    
    pegtl::argv_input<> in( &text, 0 );
    std::vector<Ohm::AST::StackItem> v;
    
    REQUIRE( pegtl::parse< 
      pegtl::must< 
          Ohm::GRM::Rule
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v ) == true );
  }
#endif  

#if 1
  SECTION( "First line of CSV example (paren+star removed) from https://github.com/harc/ohm/blob/master/examples/csv/csv.ohm" )
  {
    char *text = (char *) R"(
CSV3 {
  csv3 = row3 eol3 ~end3 row3 eol3
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
#endif  

#if 1
  SECTION( "First line of CSV example (star removed) from https://github.com/harc/ohm/blob/master/examples/csv/csv.ohm" )
  {
    char *text = (char *) R"(
CSV4 {
  csv4 = row4 (eol4 ~end4 row4) eol4
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
#endif  

#if 1
  SECTION( "Full CSV example from https://github.com/harc/ohm/blob/master/examples/csv/csv.ohm" )
  {
    char *text = (char *) R"(
CSV {
  csv = row (eol ~end row)* eol -- t
  row = col ("," col)* -- p
  col = colChar*  -- c
  colChar = ~(eol | ",") any 
  eol = "\r"? "\n"
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
#endif  
}
