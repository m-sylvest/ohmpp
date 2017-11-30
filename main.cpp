
#include "ohm-grammar.cpp"

#include <iostream>
#include <iomanip>
#include <vector>

namespace pegtl = tao::TAOCPP_PEGTL_NAMESPACE;

namespace Ohm {

	template< typename Rule >
	struct action : pegtl::nothing< Rule > 
	{
	};

	template<> 
	struct action< name >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << in.string() << std::endl;
		}
	};

	template<> 
	struct action< operator_ >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			std::cout << in.string() << std::endl;
		}
	};

	template<> 
	struct action< escapeChar >
	{
		template< typename Input >
		static void apply( const Input& in, std::vector<std::vector<int>> &v )
		{
			auto s = in.string();
			std::cout << "escapeChar(" << s.size() << ")" << std::endl;
			for (auto i = 0; i < s.length(); ++i)
				std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)s[i];

			std::cout << std::endl;
		}
	};

	template< typename Rule >
	struct control : pegtl::normal< Rule >
	{
	};

}


int main( int argc, char *argv[] )
{
  for( int i = 1; i < argc; ++i ) 
  {
      pegtl::argv_input<> in( argv, 1 );
			std::vector<std::vector<int>> v;
#if 0
      pegtl::parse< pegtl::must< Ohm::punctuation >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::tokens >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::space_ >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::STAR<Ohm::punctuation> >, Ohm::action, Ohm::control >( in, v );
      pegtl::parse< pegtl::must< Ohm::NonEmptyListOf<Ohm::STAR<Ohm::punctuation>, pegtl::one<','>> >, action, control >( in, v );
      pegtl::parse< pegtl::must< Ohm::Grammars >, action, control >( in, v );
#endif
      pegtl::parse< 
				pegtl::must< 
					pegtl::star< 
						pegtl::seq<
							pegtl::sor<Ohm::name,Ohm::escapeChar,Ohm::operator_>,
							pegtl::one<','>
						>
					>
				>, 
				Ohm::action, 
				Ohm::control 
			>( in, v );
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
