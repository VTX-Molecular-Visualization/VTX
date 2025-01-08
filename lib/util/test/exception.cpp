#include "util/exceptions.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Util::Exception", "[unit]" )
{
	using namespace VTX;

	Exception e = VTXException( "Exception" );
	CHECK( std::string( e.what() ) == "[VTX] Exception" );
	e = VTXException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[VTX] Error n.55" );

	e = GraphicException( "Exception" );
	CHECK( std::string( e.what() ) == "[GL] Exception" );
	e = GraphicException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[GL] Error n.55" );

	e = HTTPException( "Exception" );
	CHECK( std::string( e.what() ) == "[HTTP] Exception" );
	e = HTTPException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[HTTP] Error n.55" );

	e = IOException( "Exception" );
	CHECK( std::string( e.what() ) == "[IO] Exception" );
	e = IOException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[IO] Error n.55" );

	e = MathException( "Exception" );
	CHECK( std::string( e.what() ) == "[MATH] Exception" );
	e = MathException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[MATH] Error n.55" );

	e = LibException( "Exception" );
	CHECK( std::string( e.what() ) == "[LIB] Exception" );
	e = LibException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[LIB] Error n.55" );

	e = PythonWrapperException( "Exception" );
	CHECK( std::string( e.what() ) == "[WRAPPER] Exception" );
	e = PythonWrapperException( "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[WRAPPER] Error n.55" );

	e = CommandException( "MyCommand", "Exception" );
	CHECK( std::string( e.what() ) == "[COMMAND] MyCommand : Exception" );
	e = CommandException( "MyCommand", "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[COMMAND] MyCommand : Error n.55" );

	e = ScriptException( "path/script.py", "Exception" );
	CHECK( std::string( e.what() ) == "[SCRIPT] path/script.py : Exception" );
	e = ScriptException( "path/script.py", "Error n.{}", 55 );
	CHECK( std::string( e.what() ) == "[SCRIPT] path/script.py : Error n.55" );

	e = NotImplementedException();
	CHECK( std::string( e.what() ) == "[VTX] feature not implemented" );
}
