#include <catch2/catch_test_macros.hpp>
#include <concepts>

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

TEST_CASE( "Concepts usage", "[unit]" )
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
