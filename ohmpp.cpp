#include "ohm-ast-builder.hpp"
#include "ohm-ast-json.hpp"
#include "ohm-gen-grm.hpp"
#include <vector>
#include <iostream>
#include <iomanip>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"


namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

int main( int argc, char *argv[] )
{
#ifdef ARGCV
  for( int i = 1; i < argc; ++i ) 
  {
    pegtl::argv_input<> in( argv, 1 );
    std::vector<Ohm::AST::StackItem> v;
#if 0
    pegtl::parse< pegtl::must< Ohm::punctuation >, action, control >( in, v );
    pegtl::parse< pegtl::must< Ohm::tokens >, action, control >( in, v );
    pegtl::parse< pegtl::must< Ohm::space_ >, action, control >( in, v );
    pegtl::parse< pegtl::must< Ohm::STAR<Ohm::punctuation> >, Ohm::action, Ohm::control >( in, v );
    pegtl::parse< pegtl::must< Ohm::NonEmptyListOf<Ohm::STAR<Ohm::punctuation>, pegtl::one<','>> >, action, control >( in, v );
    pegtl::parse< pegtl::must< Ohm::Grammars >, action, control >( in, v );
    pegtl::parse< 
      pegtl::must< 
        pegtl::star< 
          pegtl::seq<
            pegtl::sor<Ohm::GRM::caseName,Ohm::GRM::comment,Ohm::GRM::name,Ohm::GRM::escapeChar,Ohm::GRM::operator_,Ohm::GRM::terminal>,
            pegtl::one<','>
          >
        >
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v );
#endif
#if 0
    pegtl::parse< 
      pegtl::must< 
        pegtl::star< 
          pegtl::seq<
            pegtl::sor<Ohm::GRM::Rule>,
            pegtl::one<','>
          >
        >
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v );
    Ohm::dumpStack(v);
#endif
    pegtl::parse< 
      pegtl::must< 
          Ohm::GRM::name
      >, 
      Ohm::action, 
      Ohm::control 
    >( in, v );
    Ohm::dumpStack(v);
    nlohmann::json j = nlohmann::json({ "ostemad", "havarti" });  //Ohm::JSON::to_json( v[0] );
    std::cout << j << std::endl;      
#if 0
    for( const auto& line : data ) {
       assert( !line.empty() );  // The grammar doesn't allow empty lines.
       std::cout << line.front();
       for( std::size_t j = 1; j < line.size(); ++j ) {
          std::cout << ", " << line[ j ];
       }
       std::cout << std::endl;
    }
#endif
   }
#else
  Catch::Session().run( argc, argv );

  char *text = (char *) R"(
navne { 
  Peter += Jakob* "jesper" Tina+ -- karl

  Jakob = "jakob" -- ost
  Tina  = "TINA" -- x
}
)";
  
  pegtl::argv_input<> in( &text, 0 );
  std::vector<Ohm::AST::StackItem> v;
  pegtl::parse< 
    pegtl::must< 
        Ohm::GRM::Grammars
    >, 
    Ohm::action, 
    Ohm::control 
  >( in, v );
  Ohm::dumpStack(v);
  nlohmann::json j = Ohm::JSON::to_json( v[0] );
//  std::cout << j.dump(2) << std::endl;      
  std::cout << Ohm::GenGRM::to_pegtl( v[0] );

#endif	
	return 0;
}
