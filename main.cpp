
#include "ohm-grammar.cpp"

#include <iostream>
#include <vector>

namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

template< typename Rule >
struct action : pegtl::nothing< Rule > 
{
};

template< typename Rule >
struct control : pegtl::normal< Rule >
{
};



int main( int argc, char *argv[] )
{
  for( int i = 1; i < argc; ++i ) 
  {
      pegtl::file_input<> in( argv[ i ] );
			std::vector<std::vector<int>> v;
      pegtl::parse< pegtl::must< Ohm::punctuation >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::tokens >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::space_ >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::STAR<Ohm::punctuation> >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::NonEmptyListOf<Ohm::STAR<Ohm::punctuation>, pegtl::one<','>> >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::Grammars >, action, control >( in, v );
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
	
	return 0;
}
