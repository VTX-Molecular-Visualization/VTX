#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <util/collection.hpp>
#include <util/hashing.hpp>
#include <util/singleton.hpp>

namespace
{
	class BaseClass
	{
	  public:
		BaseClass()			 = default;
		virtual ~BaseClass() = default;

		virtual int						   getValue() { return 0; };
		virtual std::unique_ptr<BaseClass> clone() const = 0;
	};

	using CollectionTest = VTX::Util::Collection<BaseClass>;
	class DerivedClass1 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass1> _reg { "DerivedClass1" };

	  public:
		DerivedClass1()						   = default;
		DerivedClass1( const DerivedClass1 & ) = default;
		virtual ~DerivedClass1()			   = default;

		int						   getValue() override { return 1; };
		std::unique_ptr<BaseClass> clone() const override { return std::make_unique<DerivedClass1>(); }
	};
	class DerivedClass2 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass2> _reg { "DerivedClass2" };

	  public:
		DerivedClass2()						   = default;
		DerivedClass2( const DerivedClass2 & ) = default;
		virtual ~DerivedClass2()			   = default;

		int						   getValue() override { return 2; };
		std::unique_ptr<BaseClass> clone() const override { return std::make_unique<DerivedClass2>(); }
	};
	class DerivedClass3 final : public BaseClass
	{
	  private:
		inline static const CollectionTest::Registration<DerivedClass3> _reg { "DerivedClass3" };

	  public:
		DerivedClass3()						   = default;
		DerivedClass3( const DerivedClass3 & ) = default;
		virtual ~DerivedClass3()			   = default;

		int						   getValue() override { return 3; };
		std::unique_ptr<BaseClass> clone() const override { return std::make_unique<DerivedClass3>(); }
	};

} // namespace

TEST_CASE( "VTX_APP - Core::Collection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	std::unique_ptr<BaseClass> ptr = nullptr;

	auto & collection = Singleton<CollectionTest>::get();

	ptr = collection.instantiateItem( "DerivedClass1" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 1 );

	ptr = collection.instantiateItem( "DerivedClass2" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 2 );

	ptr = collection.instantiateItem( "DerivedClass3" );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 3 );

	ptr = collection.instantiateItem( "DerivedClass100" );
	CHECK( ptr == nullptr );

	ptr = collection.instantiateItem( Util::hash( "DerivedClass1" ) );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 1 );

	ptr = collection.instantiateItem( Util::hash( "DerivedClass2" ) );
	REQUIRE( ptr != nullptr );
	CHECK( ptr->getValue() == 2 );

	ptr = collection.instantiateItem( Util::hash( "DerivedClass3" ) );
	CHECK( ptr != nullptr );
	CHECK( ptr->getValue() == 3 );

	ptr = collection.instantiateItem( Util::hash( "DerivedClass100" ) );
	CHECK( ptr == nullptr );

	std::unique_ptr<DerivedClass1> ptrDerivedClass1 = collection.instantiateItem<DerivedClass1>( "DerivedClass1" );
	REQUIRE( ptrDerivedClass1 != nullptr );

	ptrDerivedClass1 = collection.instantiateItem<DerivedClass1>( "DerivedClass2" );
	REQUIRE( ptrDerivedClass1 == nullptr );

	ptrDerivedClass1 = collection.instantiateItem<DerivedClass1>( Util::hash( "DerivedClass1" ) );
	REQUIRE( ptrDerivedClass1 != nullptr );

	ptrDerivedClass1 = collection.instantiateItem<DerivedClass1>( Util::hash( "DerivedClass2" ) );
	REQUIRE( ptrDerivedClass1 == nullptr );
};
