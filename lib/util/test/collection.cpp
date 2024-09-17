#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <util/collection.hpp>

namespace
{
	using namespace VTX;
	using namespace VTX::Util;

	class Base
	{
	  public:
		virtual ~Base() = default;
		// CollectionKey getName() { return "Base"; };
		virtual int getValue() { return 0; };
	};

	using CollectionTest = VTX::Util::Collection<Base *>;

	class Derived1 final : public Base //, public CollectionTest::GlobalStorage<Derived1>
	{
	  private:
		// inline static const CollectionTest::Registration<Derived1> _reg { "DerivedClass1" };

	  public:
		// CollectionKey getName() { return "DerivedClass1"; };
		int getValue() override { return 1; };
	};
	class Derived2 final : public Base //, public CollectionTest::GlobalStorage<Derived2>
	{
	  private:
		// inline static const CollectionTest::Registration<Derived2> _reg { "DerivedClass2" };

	  public:
		// CollectionKey getName() { return "DerivedClass2"; };
		int getValue() override { return 2; };
	};
	class Derived3 final : public Base
	{
	  private:
		// inline static const CollectionTest::Registration<Derived3> _reg { "DerivedClass3" };

	  public:
		// CollectionKey getName() { return "DerivedClass3"; };
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

	REQUIRE( colPtr.has<Derived3>() );

	colPtr.remove<Derived1>();
	colPtr.remove<Derived2>();
	colPtr.remove<Derived3>();

	REQUIRE( not colPtr.has<Derived1>() );
	REQUIRE( not colPtr.has<Derived2>() );
	REQUIRE( not colPtr.has<Derived3>() );

	// Smart ptr.
	Collection<std::unique_ptr<Base>> colSmartPtr;

	derived1 = colSmartPtr.create<Derived1>();
	derived2 = colSmartPtr.create<Derived2>();

	REQUIRE( derived1 );
	REQUIRE( derived2 );

	// CollectionTest & collection = Singleton<CollectionTest>::get();

	// Check globale storage.
	/*
	CHECK( collection.has<Derived1>() );
	CHECK( collection.has<Derived2>() );
	CHECK( not collection.has<Derived3>() );

	collection.remove<Derived1>();
	collection.remove<Derived2>();
	*/

	// Using key.
	/*
	derived1 = collection.createFromKey<Derived1>( "DerivedClass1" );
	REQUIRE( derived1 );
	CHECK( derived1->getValue() == 1 );

	derived2 = collection.createFromKey<Derived2>( "DerivedClass2" );
	REQUIRE( derived2 );
	CHECK( derived2->getValue() == 2 );

	derived3 = collection.createFromKey<Derived3>( "DerivedClass3" );
	REQUIRE( derived3 );
	CHECK( derived3->getValue() == 3 );

	collection.remove<Derived3>();

	// Polymorphism.
	Base * base;
	base = collection.createFromKey<Derived3>( "DerivedClass3" );
	REQUIRE( base );
	CHECK( base->getValue() == 3 );
	*/
};
