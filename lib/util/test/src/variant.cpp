#include <catch2/catch_test_macros.hpp>
#include <string>
#include <util/variant.hpp>

class TestClass
{
  public:
	TestClass() = default;
	TestClass( const int p_int ) : _int( p_int ) {}
	TestClass( const TestClass & ) = default;
	~TestClass()				   = default;

	int	 getInt() const { return _int; }
	void setInt( int p_int ) { _int = p_int; }

  private:
	int _int = 10;
};

// string.hpp
TEST_CASE( "Util::Variant", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	VTXVariant variant;

	variant = VTXVariant( true );
	CHECK( variant.is<bool>() );
	CHECK( variant.get<bool>() == true );

	variant = VTXVariant( 10 );
	CHECK( variant.is<int>() );
	CHECK( variant.get<int>() == 10 );

	variant = VTXVariant( uint( -10 ) );
	CHECK( variant.is<uint>() );
	CHECK( variant.get<uint>() == uint( -10 ) );

	variant = VTXVariant( 10.f );
	CHECK( variant.is<float>() );
	CHECK( variant.get<float>() == 10.f );

	variant = VTXVariant( 10. );
	CHECK( variant.is<double>() );
	CHECK( variant.get<double>() == 10. );

	variant = VTXVariant( "string" );
	CHECK( variant.is<std::string>() );
	CHECK( variant.get<std::string>() == "string" );

	variant = VTXVariant( std::string( "string" ) );
	CHECK( variant.is<std::string>() );
	CHECK( variant.get<std::string>() == "string" );

	std::unique_ptr<TestClass> testClassPtr = std::make_unique<TestClass>( 10 );

	variant = VTXVariant( testClassPtr.get() );
	CHECK( variant.isPtr() );
	CHECK( variant.getPtr<TestClass>() == testClassPtr.get() );
}
