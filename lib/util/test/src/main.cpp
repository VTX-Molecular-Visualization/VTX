#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <fstream>
#include <util/algorithm/range.hpp>
#include <util/enum.hpp>
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
	CHECK( str == "test   " );

	str = "   test   ";
	VTX::Util::String::trimEnd( str );
	CHECK( str == "   test" );

	str = "   test   ";
	VTX::Util::String::trim( str );
	CHECK( str == "test" );

	str = "a string with characters to replace";
	VTX::Util::String::replaceAll( str, "r", "t" );
	CHECK( str == "a stting with chatactets to teplace" );

	const float f = 3.14159;
	CHECK( VTX::Util::String::floatToStr( f, 0 ) == "3" );
	CHECK( VTX::Util::String::floatToStr( f, 2 ) == "3.14" );
	CHECK( VTX::Util::String::floatToStr( f, 5 ) == "3.14159" );

	str = "3.14159";
	CHECK( VTX::Util::String::strToUint( str ) == 3 );

	str = "123 abcDefghijklmnopqrstuvwxyZ.()+";
	VTX::Util::String::toUpper( str );
	CHECK( str == "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ.()+" );
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
	CHECK( VTX::Util::Filesystem::readPath( filePath ) == "Hello, world!" );

	VTX::FilePath filePath2( "data_test/test.txt" );
	VTX::Util::Filesystem::generateUniqueFileName( filePath2 );

	REQUIRE( filePath2.filename().string() == "test_2.txt" );
	CHECK( filePath2.string() == VTX::FilePath( "data_test" ) / "test_2.txt" );

	std::ofstream file2( filePath2 );
	file2 << "Hello, world!";
	file2.close();

	VTX::Util::Filesystem::removeAll( "data_test/sdqsdqsd" );
	CHECK( std::filesystem::exists( "data_test" ) == false );
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
	CHECK( !range.isValid() );

	range = Util::Math::Range<size_t>( 6 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 6 );
	CHECK( range.getCount() == 1 );
	CHECK( range.getLast() == 6 );

	range = Util::Math::Range<size_t>( 50, 5 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 50 );
	CHECK( range.getCount() == 5 );
	CHECK( range.getLast() == 54 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getCount() == 11 );
	CHECK( range.getLast() == 20 );

	range.add( 3 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 23 );

	range.remove( 8 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.setFirst( 15 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 15 );
	CHECK( range.getLast() == 20 );

	range.setLast( 15 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 15 );
	CHECK( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 12 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 5 );
	CHECK( range.getLast() == 20 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 19, 27 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 27 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 25 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 5 );
	CHECK( range.getLast() == 25 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 25, 35 ) );
	CHECK( !range.isValid() );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	CHECK( range.contains( 10 ) );
	CHECK( range.contains( 15 ) );
	CHECK( range.contains( 20 ) );
	CHECK( !range.contains( 9 ) );
	CHECK( !range.contains( 21 ) );

	CHECK( range.contains( { 10, 12, 18 } ) );
	CHECK( !range.contains( { 10, 12, 18, 22 } ) );

	CHECK( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 20 ) ) );
	CHECK( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 15 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 9, 24 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 1, 5 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 12, 24 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 500, 5000 ) ) );

	CHECK( range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
							 Util::Math::Range<size_t>::createFirstLast( 14, 18 ) } ) );
	CHECK( !range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
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

	CHECK( itemCount == 22 );

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

	CHECK( itemCount == 3 );

	rangeList = Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
												 Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
												 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	CHECK( rangeList.contains( 7 ) );
	CHECK( !rangeList.contains( 10 ) );

	CHECK( rangeList.contains( { 7, 13, 50 } ) );
	CHECK( !rangeList.contains( { 7, 13, 50, 52 } ) );

	CHECK( rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 20 ) ) );
	CHECK( !rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 50 ) ) );

	CHECK( rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
								 Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
								 Util::Math::Range<size_t>::createFirstLast( 12, 14 ),
								 Util::Math::Range<size_t>::createFirstLast( 18, 20 ) } ) );

	CHECK( !rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
								  Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
								  Util::Math::Range<size_t>::createFirstLast( 7, 14 ) } ) );

	Util::Math::RangeList<size_t> rangeListA
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
										   Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	Util::Math::RangeList<size_t> rangeListB
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
										   Util::Math::Range<size_t>::createFirstLast( 4, 5 ),
										   Util::Math::Range<size_t>::createFirstLast( 7, 14 ),
										   Util::Math::Range<size_t>::createFirstLast( 18, 19 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	// Check Merges
	Util::Math::RangeList<size_t> rangeListRes1 = Util::Algorithm::Range::merge( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 20 ),
									 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } ) );

	Util::Math::RangeList<size_t> rangeListRes2 = Util::Algorithm::Range::merge( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::mergeInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Substract
	rangeListRes1 = Util::Algorithm::Range::substract( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 6, 6 ),
									 Util::Math::Range<size_t>::createFirstLast( 15, 17 ),
									 Util::Math::Range<size_t>::createFirstLast( 20, 20 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::substract( rangeListB, rangeListA );
	CHECK( rangeListRes2.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 4 ),
									 Util::Math::Range<size_t>::createFirstLast( 9, 11 ) } ) );

	rangeListRes2 = rangeListA;
	Util::Algorithm::Range::substractInSitu( rangeListRes2, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Intersect
	rangeListRes1 = Util::Algorithm::Range::intersect( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 5 ),
									 Util::Math::Range<size_t>::createFirstLast( 7, 8 ),
									 Util::Math::Range<size_t>::createFirstLast( 12, 14 ),
									 Util::Math::Range<size_t>::createFirstLast( 18, 19 ),
									 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::intersect( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::intersectInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Exclusive
	rangeListRes1 = Util::Algorithm::Range::exclusive( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 4 ),
									 Util::Math::Range<size_t>::createFirstLast( 15, 17 ),
									 Util::Math::Range<size_t>::createFirstLast( 20, 20 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::exclusive( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::exclusiveInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );
};

// enum.hpp
enum struct E_EXAMPLE_1
{
	FIRST,
	SECOND
};

enum struct E_EXAMPLE_2
{
	FIRST = 42,
	OTHER,
	SECOND
};

TEST_CASE( "Util::Enum", "[enum]" )
{
	using namespace VTX::Util;

	CHECK( Enum::enumName( E_EXAMPLE_1::FIRST ).compare( "FIRST" ) == 0 );
	CHECK( Enum::enumCast<E_EXAMPLE_1>( "SECOND" ) == E_EXAMPLE_1::SECOND );
	CHECK( Enum::enumToAnother<E_EXAMPLE_1, E_EXAMPLE_2>( E_EXAMPLE_1::SECOND ) == E_EXAMPLE_2::SECOND );
	CHECK( Enum::enumInteger( Enum::enumToAnother<E_EXAMPLE_1, E_EXAMPLE_2>( E_EXAMPLE_1::SECOND ) ) == 44 );
}

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
