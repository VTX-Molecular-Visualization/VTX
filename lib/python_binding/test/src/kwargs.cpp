#include "python_binding/interpretor.hpp"
#include "util/app.hpp"
#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <python_binding/api/arguments.hpp>
#include <python_binding/binding/vtx_module.hpp>

namespace Test
{
	struct PommeDapis
	{
		std::string fruit;
		friend bool operator==( const PommeDapis &, const PommeDapis & ) = default;
	};
	PommeDapis convert( const std::string & s ) { return PommeDapis { s }; }
	struct TestStruct
	{
		size_t					 size = 0;
		std::string				 string;
		std::vector<std::string> strings;
		std::vector<size_t>		 uint64s;
		std::vector<uint32_t>	 uint32s;
		std::vector<PommeDapis>	 items;
		bool					 containShit = false;
	};

} // namespace Test

TEST_CASE( "VTX_PYTHON_BINDING - Kwarg wrapper test", "[python][kwargs]" )
{
	VTX::App::Test::Util::PythonFixture f;

	pybind11::module_ * mod = nullptr;
	VTX::INTERPRETOR().getModule().commands().getPythonModule( &mod );

	Test::TestStruct test;

	VTX::INTERPRETOR().getModule().commands().def(
		"test_kwargs",
		[ & ]( const pybind11::kwargs & p_kwargs )
		{
			VTX::PythonBinding::API::PythonKwargs kwargs( p_kwargs );
			test.size		 = kwargs.size();
			test.containShit = kwargs.contains( "Shit" );
			kwargs.get( "names", test.strings );
			kwargs.get( "name", test.string );
			kwargs.get( "numbers", test.uint32s );
			kwargs.get( "bigNumbers", test.uint64s );
			kwargs.get( "weird", &Test::convert, test.items );
		},
		"Test for keyword arguments"
	);

	pybind11::exec( fmt::format( "from {}.Command import *", VTX::PythonBinding::vtx_module_name() ) );

	pybind11::exec( "test_kwargs(name='poney')" );
	CHECK( test.size == 1 );
	CHECK( test.containShit == false );
	CHECK( test.string == "poney" );
	CHECK( test.strings.empty() );
	CHECK( test.uint64s.empty() );
	CHECK( test.uint32s.empty() );
	CHECK( test.items.empty() );

	pybind11::exec( "test_kwargs(names=('poney', 'horse'))" );
	CHECK( test.size == 1 );
	CHECK( test.containShit == false );
	CHECK( test.string.empty() );
	CHECK( test.strings.size() == 2 );
	CHECK( std::find( test.strings.begin(), test.strings.end(), std::string( "poney" ) ) != std::end( test.strings ) );
	CHECK( std::find( test.strings.begin(), test.strings.end(), std::string( "horse" ) ) != std::end( test.strings ) );
	CHECK( test.uint64s.empty() );
	CHECK( test.uint32s.empty() );
	CHECK( test.items.empty() );

	pybind11::exec(
		"test_kwargs(name='poney', names=('poney','horse'), numbers=(5,54), bigNumbers=(18446744073709551613, "
		"18446744073709551610), weird=('pinegrape', 'dragonfruit'), notOverlooked='meh', Shit='none')"
	);
	CHECK( test.size == 7 );
	CHECK( test.containShit == true );
	CHECK( test.string == "poney" );
	CHECK( test.strings.size() == 2 );
	CHECK( test.uint32s.size() == 2 );
	CHECK( test.uint64s.size() == 2 );
	CHECK( std::find( test.uint64s.begin(), test.uint64s.end(), 18446744073709551613 ) != std::end( test.uint64s ) );
	CHECK( std::find( test.uint64s.begin(), test.uint64s.end(), 18446744073709551610 ) != std::end( test.uint64s ) );
	CHECK( test.items.size() == 2 );
	CHECK(
		std::find( test.items.begin(), test.items.end(), Test::PommeDapis { "pinegrape" } ) != std::end( test.items )
	);
	CHECK(
		std::find( test.items.begin(), test.items.end(), Test::PommeDapis { "dragonfruit" } ) != std::end( test.items )
	);

	pybind11::exec( "test_kwargs()" );
	CHECK( test.size == 0 );
	CHECK( test.containShit == false );
	CHECK( test.string.empty() );
	CHECK( test.strings.empty() );
	CHECK( test.uint64s.empty() );
	CHECK( test.uint32s.empty() );
	CHECK( test.items.empty() );
}
