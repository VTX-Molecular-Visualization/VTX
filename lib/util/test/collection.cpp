#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <util/collection.hpp>

namespace
{

	class Base
	{
	  public:
		virtual int getValue() { return 0; };
	};

	using CollectionTest = VTX::Util::Collection<std::unique_ptr<Base>>;

	class Derived1 final : public Base
	{
	  private:
		inline static const CollectionTest::Registration<Derived1> _reg { "DerivedClass1" };

	  public:
		int getValue() override { return 1; };
	};
	class Derived2 final : public Base
	{
	  private:
		inline static const CollectionTest::Registration<Derived2> _reg { "DerivedClass2" };

	  public:
		int getValue() override { return 2; };
	};
	class Derived3 final : public Base
	{
	  private:
		inline static const CollectionTest::Registration<Derived3> _reg { "DerivedClass3" };

	  public:
		int getValue() override { return 3; };
	};

} // namespace

TEST_CASE( "VTX_APP - Core::HahsedCollection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	// Raw ptr.
	Collection<Base *> colPtr;

	Derived1 * derived1 = colPtr.create<Derived1>();
	Derived2 * derived2 = colPtr.create<Derived2>();

	REQUIRE( derived1 );
	REQUIRE( derived2 );

	Derived3 * derived3 = new Derived3();

	colPtr.set<Derived3>( derived3 );

	// Smart ptr.
	Collection<std::unique_ptr<Base>> colSmartPtr;

	derived1 = colSmartPtr.create<Derived1>();
	derived2 = colSmartPtr.create<Derived2>();

	REQUIRE( derived1 );
	REQUIRE( derived2 );
	///////////////////////////////////////

	CollectionTest & collection = Singleton<CollectionTest>::get();

	derived1 = collection.createFromKey<Derived1>( "DerivedClass1" );
	REQUIRE( derived1 );
	CHECK( derived1->getValue() == 1 );

	derived2 = collection.createFromKey<Derived2>( "DerivedClass2" );
	REQUIRE( derived2 );
	CHECK( derived2->getValue() == 2 );

	derived3 = collection.createFromKey<Derived3>( "DerivedClass3" );
	REQUIRE( derived3 );
	CHECK( derived3->getValue() == 3 );

	Base * base;
	base = collection.createFromKey<Derived3>( "DerivedClass3" );
	REQUIRE( base );
	CHECK( base->getValue() == 3 );
};
