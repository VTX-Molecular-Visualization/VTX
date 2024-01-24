#include <app/core/collection.hpp>
#include <app/core/collectionable.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <util/hashing.hpp>

namespace
{
	using Collectionable = VTX::App::Core::Collectionable;
	class BaseClass : public Collectionable
	{
	  public:
		BaseClass()			 = default;
		virtual ~BaseClass() = default;

		virtual int						   getValue() { return 0; };
		virtual std::unique_ptr<BaseClass> clone() = 0;
	};

	using CollectionTest = VTX::App::Core::Collection<BaseClass>;
	class DerivedClass1 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass1> _reg { "DerivedClass1" };

	  public:
		DerivedClass1()						   = default;
		DerivedClass1( const DerivedClass1 & ) = default;
		virtual ~DerivedClass1()			   = default;

		int getValue() override { return 1; };

		std::unique_ptr<BaseClass> clone() override { return std::make_unique<DerivedClass1>( *this ); };
	};
	class DerivedClass2 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass2> _reg { "DerivedClass2" };

	  public:
		DerivedClass2()						   = default;
		DerivedClass2( const DerivedClass2 & ) = default;
		virtual ~DerivedClass2()			   = default;

		int getValue() override { return 2; };

		std::unique_ptr<BaseClass> clone() override { return std::make_unique<DerivedClass2>( *this ); };
	};
	class DerivedClass3 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass3> _reg { "DerivedClass3" };

	  public:
		DerivedClass3()						   = default;
		DerivedClass3( const DerivedClass3 & ) = default;
		virtual ~DerivedClass3()			   = default;

		int getValue() override { return 3; };

		std::unique_ptr<BaseClass> clone() override { return std::make_unique<DerivedClass3>( *this ); };
	};

} // namespace

TEST_CASE( "VTX_APP - Core::Collection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using Collectionable = VTX::App::Core::Collectionable;

	std::unique_ptr<BaseClass> ptr = nullptr;

	ptr = CollectionTest::get().instantiateItem( "DerivedClass1" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 1 );

	ptr = CollectionTest::get().instantiateItem( "DerivedClass2" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 2 );

	ptr = CollectionTest::get().instantiateItem( "DerivedClass3" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 3 );

	ptr = CollectionTest::get().instantiateItem( "DerivedClass100" );
	CHECK( ptr == nullptr );

	ptr = CollectionTest::get().instantiateItem( Util::Hashing::hash( "DerivedClass1" ) );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 1 );

	ptr = CollectionTest::get().instantiateItem( Util::Hashing::hash( "DerivedClass2" ) );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 2 );

	ptr = CollectionTest::get().instantiateItem( Util::Hashing::hash( "DerivedClass3" ) );
	CHECK( ptr != nullptr );
	CHECK( ptr->getValue() == 3 );

	ptr = CollectionTest::get().instantiateItem( Util::Hashing::hash( "DerivedClass100" ) );
	CHECK( ptr == nullptr );

	std::unique_ptr<DerivedClass1> ptrDerivedClass1
		= CollectionTest::get().instantiateItem<DerivedClass1>( "DerivedClass1" );
	REQUIRE( ptrDerivedClass1 != nullptr );

	ptrDerivedClass1 = CollectionTest::get().instantiateItem<DerivedClass1>( "DerivedClass2" );
	REQUIRE( ptrDerivedClass1 == nullptr );

	ptrDerivedClass1 = CollectionTest::get().instantiateItem<DerivedClass1>( Util::Hashing::hash( "DerivedClass1" ) );
	REQUIRE( ptrDerivedClass1 != nullptr );

	ptrDerivedClass1 = CollectionTest::get().instantiateItem<DerivedClass1>( Util::Hashing::hash( "DerivedClass2" ) );
	REQUIRE( ptrDerivedClass1 == nullptr );
};
