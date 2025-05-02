#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/api/collection.hpp>
#include <vector>

namespace
{
	class Element
	{
	  public:
		int i = 0;
	};
	class ElementWrapper
	{
		Element * _ptr = nullptr;

	  public:
		ElementWrapper() = default;
		ElementWrapper( Element & p_ ) : _ptr( &p_ ) {}

		inline int geti() const
		{
			if ( _ptr )
				return _ptr->i;
			return 0;
		}
	};
} // namespace

TEST_CASE( "VTX_PYTHON_BINDING - Command binding test", "[collection]" )
{
	using namespace VTX::PythonBinding::API;

	std::vector<Element> vec { { 1 }, { 2 }, { 3 } };

	Collection<ElementWrapper> collec { vec };

	CHECK( collec[ 0 ].geti() == 1 );
	CHECK( collec[ 1 ].geti() == 2 );
	CHECK( collec[ 2 ].geti() == 3 );

	auto a = collec.end();
	{
		int expected_i = 1;
		for ( auto & it : collec )
		{
			CHECK( it.geti() == expected_i );
			expected_i++;
		}
		size_t it_idx = 0;
		for ( auto it = collec.begin(); it != collec.end(); it++ )
		{
			CHECK( ( std::distance( collec.begin(), it ) == it_idx ) );
			it_idx++;
		}
		CHECK( it_idx == collec.size() );
	}
}
