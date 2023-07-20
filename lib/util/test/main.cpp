#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <fstream>
#include <util/filesystem.hpp>
#include <util/generic/base_static_singleton.hpp>
#include <util/logger.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/string.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[logger]" )
{
	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
}

// string.hpp
TEST_CASE( "Util::String", "[string]" )
{
	std::string str = "   test   ";
	VTX::Util::String::trimStart( str );
	REQUIRE( str == "test   " );

	str = "   test   ";
	VTX::Util::String::trimEnd( str );
	REQUIRE( str == "   test" );

	str = "   test   ";
	VTX::Util::String::trim( str );
	REQUIRE( str == "test" );

	str = "a string with characters to replace";
	VTX::Util::String::replaceAll( str, "r", "t" );
	REQUIRE( str == "a stting with chatactets to teplace" );

	const float f = 3.14159;
	REQUIRE( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	REQUIRE( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	REQUIRE( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );

	str = "3.14159";
	REQUIRE( VTX::Util::String::strToUint( str ) == 3 );
}

// filesystem.hpp
TEST_CASE( "Util::Filesystem", "[filesystem]" )
{
	std::filesystem::create_directory( "data_test" );
	const VTX::FilePath filePath( "data_test/test.txt" );
	std::ofstream		file( filePath );
	file << "Hello, world!";
	file.close();

	REQUIRE( std::filesystem::exists( filePath ) );
	REQUIRE( VTX::Util::Filesystem::readPath( filePath ) == "Hello, world!" );

	VTX::FilePath filePath2( "data_test/test.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath2 );

	REQUIRE( filePath2.filename().string() == "test_2.txt" );
	REQUIRE( filePath2.string() == VTX::FilePath( "data_test" ) / "test_2.txt" );

	std::ofstream file2( filePath2 );
	file2 << "Hello, world!";
	file2.close();

	VTX::Util::Filesystem::removeAll( "data_test/sdqsdqsd" );
	REQUIRE( std::filesystem::exists( "data_test" ) == false );
}

// base_static_singleton.hpp
TEST_CASE( "Util::BaseStaticSingleton", "[generic]" )
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

TEST_CASE( "Util::Math::Range", "[math]" )
{
	using namespace VTX;

	Util::Math::Range<size_t> range;
	REQUIRE( !range.isValid() );

	range = Util::Math::Range<size_t>( 6 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 6 );
	REQUIRE( range.getCount() == 1 );
	REQUIRE( range.getLast() == 6 );

	range = Util::Math::Range<size_t>( 50, 5 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 50 );
	REQUIRE( range.getCount() == 5 );
	REQUIRE( range.getLast() == 54 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 10 );
	REQUIRE( range.getCount() == 11 );
	REQUIRE( range.getLast() == 20 );

	range.add( 3 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 10 );
	REQUIRE( range.getLast() == 23 );

	range.remove( 8 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 10 );
	REQUIRE( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.setFirst( 15 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 15 );
	REQUIRE( range.getLast() == 20 );

	range.setLast( 15 );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 15 );
	REQUIRE( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 12 ) );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 5 );
	REQUIRE( range.getLast() == 20 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 19, 27 ) );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 10 );
	REQUIRE( range.getLast() == 27 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 25 ) );
	REQUIRE( range.isValid() );
	REQUIRE( range.getFirst() == 5 );
	REQUIRE( range.getLast() == 25 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 25, 35 ) );
	REQUIRE( !range.isValid() );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	REQUIRE( range.contains( 10 ) );
	REQUIRE( range.contains( 15 ) );
	REQUIRE( range.contains( 20 ) );
	REQUIRE( !range.contains( 9 ) );
	REQUIRE( !range.contains( 21 ) );

	REQUIRE( range.contains( { 10, 12, 18 } ) );
	REQUIRE( !range.contains( { 10, 12, 18, 22 } ) );

	REQUIRE( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 20 ) ) );
	REQUIRE( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 15 ) ) );
	REQUIRE( !range.contains( Util::Math::Range<size_t>::createFirstLast( 9, 24 ) ) );
	REQUIRE( !range.contains( Util::Math::Range<size_t>::createFirstLast( 1, 5 ) ) );
	REQUIRE( !range.contains( Util::Math::Range<size_t>::createFirstLast( 12, 24 ) ) );
	REQUIRE( !range.contains( Util::Math::Range<size_t>::createFirstLast( 500, 5000 ) ) );

	REQUIRE( range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
							   Util::Math::Range<size_t>::createFirstLast( 14, 18 ) } ) );
	REQUIRE( !range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
								Util::Math::Range<size_t>::createFirstLast( 14, 18 ),
								Util::Math::Range<size_t>::createFirstLast( 19, 22 ) } ) );
};
TEST_CASE( "Util::Math::RangeList", "[math]" )
{
	using namespace VTX;

	Util::Math::RangeList<size_t> rangeList
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
										   Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 9, 10 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 8, 14 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 51, 55 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 6, 7 ) );

	int itemCount = 0;
	for ( size_t index : rangeList )
		itemCount++;

	REQUIRE( itemCount == 22 );

	rangeList = Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
												 Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
												 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 1, 4 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 10, 14 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 6, 7 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 1, 18 ) );

	itemCount = 0;
	for ( size_t index : rangeList )
		itemCount++;

	REQUIRE( itemCount == 3 );

	rangeList = Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
												 Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
												 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	REQUIRE( rangeList.contains( 7 ) );
	REQUIRE( !rangeList.contains( 10 ) );

	REQUIRE( rangeList.contains( { 7, 13, 50 } ) );
	REQUIRE( !rangeList.contains( { 7, 13, 50, 52 } ) );

	REQUIRE( rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 20 ) ) );
	REQUIRE( !rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 50 ) ) );

	REQUIRE( rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
								   Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
								   Util::Math::Range<size_t>::createFirstLast( 12, 14 ),
								   Util::Math::Range<size_t>::createFirstLast( 18, 20 ) } ) );

	REQUIRE( !rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
									Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
									Util::Math::Range<size_t>::createFirstLast( 7, 14 ) } ) );
};

// C++20 static polymorphism with concepts.
template<typename T>
concept canUse = requires( T t ) {
					 {
						 t.use()
						 } -> std::same_as<void>;
				 };

template<canUse T>
class BaseClass : public T
{
  public:
	void baseMethod() {}
};

class DerivedClass
{
  public:
	void use() {}
	void derivedMethod() {}
};

TEST_CASE( "Concepts usage", "[c++20]" )
{
	using MyClass = BaseClass<DerivedClass>;
	MyClass c;
	c.use();
	c.baseMethod();
	c.derivedMethod();

	DerivedClass d;
	d.use();
	// d.baseMethod(); // Forbidden.
	d.derivedMethod();
}
