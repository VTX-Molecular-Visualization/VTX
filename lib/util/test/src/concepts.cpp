#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <list>
#include <set>
#include <string_view>
#include <unordered_set>
#include <util/concepts.hpp>
#include <vector>

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

template<VTX::Container Container>
void containerFunc( const Container & p_container )
{
	size_t containerSize = p_container.size();

	size_t counter = 0;
	for ( typename Container::const_iterator it = p_container.begin(); it != p_container.end(); it++ )
	{
		counter++;
	}
	CHECK( containerSize == counter );
}

template<VTX::ContainerOfType<int> Container>
void containerOfTypeFunc( Container & p_container )
{
	size_t containerSize = p_container.size();

	// for ( typename Container::iterator it = p_container.begin(); it != p_container.end(); it++ )
	//{
	//	*it = ( *it ) * 2;
	// }

	size_t sum = 0;
	for ( typename Container::const_iterator it = p_container.cbegin(); it != p_container.cend(); it++ )
	{
		sum += *it;
	}
}

template<VTX::EnumConcept E>
E enumFunc( const E p_enum )
{
	return E( int( p_enum ) + 1 );
}

template<VTX::StringConcept S>
void stringFunc( const S & p_str )
{
	std::string str = std::string( p_str );
}

enum class EnumTest : int
{
	ONE,
	TWO,
	THREE
};

TEST_CASE( "Util::Concepts", "[unit]" )
{
	std::vector<int>		vecInt			= { 0, 1, 2, 3, 4, 5 };
	std::list<int>			listInt			= { 0, 1, 2, 3, 4, 5 };
	std::set<int>			setInt			= { 0, 1, 2, 3, 4, 5 };
	std::unordered_set<int> unorderedSetInt = { 0, 1, 2, 3, 4, 5 };

	containerFunc( vecInt );
	containerFunc( listInt );
	containerFunc( setInt );
	containerFunc( unorderedSetInt );

	containerOfTypeFunc( vecInt );
	containerOfTypeFunc( listInt );
	containerOfTypeFunc( setInt );
	containerOfTypeFunc( unorderedSetInt );

	EnumTest enumtest = EnumTest::TWO;
	enumtest		  = enumFunc( enumtest );
	CHECK( enumtest == EnumTest::THREE );

	stringFunc( std::string( "Youpi" ) );
	stringFunc( "Youpi" );
	stringFunc( std::string_view( "Youpi" ) );
}
