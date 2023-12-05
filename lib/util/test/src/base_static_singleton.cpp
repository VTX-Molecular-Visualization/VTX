#include <catch2/catch_test_macros.hpp>
#include <util/generic/base_static_singleton.hpp>

// base_static_singleton.hpp
TEST_CASE( "Util::BaseStaticSingleton", "[unit]" )
{
	class SingletonTest : public VTX::Util::Generic::BaseStaticSingleton<SingletonTest>
	{
	  public:
		explicit SingletonTest( const StructPrivacyToken & ) {}
		SingletonTest( std::initializer_list<int> ) = delete;
	};

	const SingletonTest & instance = VTX::Util::Generic::BaseStaticSingleton<SingletonTest>::get();

	// Forbidden.
	// const SingletonTest consDefault;
	// const SingletonTest consCopy( instance );
	// const SingletonTest consMove( std::move( instance ) );
	// const SingletonTest consInit( {} );
}
