#include "external_tool/action.hpp"
#include "external_tool/binding.hpp"
#include "util/app.hpp"
#include <python_binding/api/api.hpp>

// #include <app/filesystem.hpp>
// #include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <pybind11/embed.h>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/object.hpp>
#include <source_location>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

std::string src_info( const std::source_location location = std::source_location::current() )
{
	return std::string( "[" ) + location.file_name() + '(' + std::to_string( location.line() ) + ':'
		   + std::to_string( location.column() ) + " '" + location.function_name() + "']";
}

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][action]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.addBinder<VTX::Test::ExternalTool::Binder>();
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == false );

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.runCommand( "ToolActionExecute()" );
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == true );
}

namespace VTX::Test
{
	using namespace VTX::PythonBinding::API;
	struct MockResidue;
	struct MockSystem;
	struct MockChain;
	struct MockAtom;
	struct MockResidue
	{
		uint64_t	 index			 = 0;
		atom_index_t index_firstAtom = 0;
		atom_index_t index_lastAtom	 = 0;

		size_t getIndex() const { return index; }
		void   setIndex( const size_t p_index ) { index = p_index; }

		atom_index_t getIndexFirstAtom() const { return index_firstAtom; }
		void		 setIndexFirstAtom( const atom_index_t p_indexFirstAtom ) { index_firstAtom = p_indexFirstAtom; }
		atom_index_t getIndexLastAtom() const { return index_lastAtom; }

		atom_index_t getAtomCount() const							= 0;
		void		 setAtomCount( const atom_index_t p_atomCount ) = 0;

		size_t getIndexFirstBond() const						  = 0;
		void   setIndexFirstBond( const size_t p_indexFirstBond ) = 0;

		size_t getBondCount() const						= 0;
		void   setBondCount( const size_t p_bondCount ) = 0;

		size_t getIndexInOriginalChain() const = 0;

		const std::string_view getShortName() const = 0;
		const std::string_view getName() const		= 0;
		const std::string_view getLongName() const	= 0;

		void setIndexInOriginalChain( const size_t p_index ) = 0;

		void setVisible( const bool p_visible ) = 0;
		bool isVisible() const					= 0;
		bool isFullyVisible() const				= 0;

		const Chain	 getChain() const  = 0;
		Chain		 getChain()		   = 0;
		const System getSystem() const = 0;
		System		 getSystem()	   = 0;
	};

	struct MockAtom
	{
		atom_index_t			   index   = 0;
		MockResidue *			   residue = nullptr;
		MockChain *				   chain   = nullptr;
		MockSystem *			   system  = nullptr;
		std::string				   name;
		Core::ChemDB::Atom::SYMBOL symbol = Core::ChemDB::Atom::SYMBOL::UNKNOWN;
		Core::ChemDB::Atom::TYPE   type	  = Core::ChemDB::Atom::TYPE::NORMAL;
		Vec3f					   position { 0.f, 0.f, 0.f };
		bool					   visible	 = false;
		bool					   removed	 = false;
		float					   vdwRadius = 1.5f;

		atom_index_t getIndex() const
		{
			//
			return index;
			//
		}
		void				setIndex( const atom_index_t p_index ) { index = p_index; }
		MockResidue *		getResiduePtr() { return residue; }
		const MockResidue * getConstResiduePtr() const { return residue; }
		MockChain *			getChainPtr() { return chain; }
		const MockChain *	getConstChainPtr() const { return chain; }
		MockSystem *		getSystemPtr() { return system; }
		const MockSystem *	getConstSystemPtr() const { return system; }

		const std::string &				   getName() const { return name; }
		void							   setName( const std::string & p_name ) { name = p_name; }
		const Core::ChemDB::Atom::SYMBOL & getSymbol() const { return symbol; }
		void setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol ) { symbol = p_symbol; }

		Core::ChemDB::Atom::TYPE getType() const { return type; }
		void					 setType( const Core::ChemDB::Atom::TYPE p_type ) { type = p_type; }

		float getVdwRadius() const { return vdwRadius; }

		const Vec3f & getLocalPosition() const { return position; }
		Vec3f		  getWorldPosition() const { return position - Vec3f { 1.f, 1.f, 1.f }; }

		bool isVisible() const { return visible; }
		void setVisible( const bool p_visible ) { visible = p_visible; }

		void remove() { removed = true; }
	};
} // namespace VTX::Test

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][api]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	pybind11::module_ * vtxModule = nullptr;
	interpretor.getPythonModule( &vtxModule );
	Test::MockAtom mockedAtom {};

	interpretor.getModule().api().getPythonModule( &vtxModule );
	vtxModule->def(
		"TEST_getSampleAtom",
		[ & ]()
		{
			return PythonBinding::API::Atom( mockedAtom );
			/*
			pybind11::object rr {
				pybind11::cast( PythonBinding::API::Atom( mockedAtom ), pybind11::return_value_policy::move )
			};
			return rr;
			*/
		}
		//{ return std::make_unique<PythonBinding::API::Atom>( mockedAtom ); }
		,
		pybind11::return_value_policy::move
	);
	pybind11::exec( "from vtx_python_api.API import *" );
	pybind11::exec( "print(dir(vtx_python_api.API))" );
	auto returnedAtom = pybind11::eval( "vtx_python_api.API.TEST_getSampleAtom()" );
	CHECK( returnedAtom.attr( "getIndex" )().cast<uint64_t>() == mockedAtom.index );
}

TEST_CASE( "VTX_PYTHON_BINDING - Module loading", "[python][binding][module]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	const FilePath scriptPath
		= VTX::Util::Filesystem::getExecutableDir()
		  / "data" // This one is technically a constant duplication, but it is placed at a sub-optimal location
				   // throughout VTX (i.e. App) and is not accessible. Maybe this shall be moved toward Util instead.
		  / "custom_module.py";
	REQUIRE( std::filesystem::exists( scriptPath ) == true );
	const PythonBinding::Wrapper::Module customModule = interpretor.loadModule( scriptPath );

	customModule.displayInfo();

	try
	{
		customModule.runFunction<std::string>( "testStr" );
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<std::string>( "testStr", "VTX" );
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<int>( "iDontExist", "from VTX" );
		CHECK( false );
	}
	catch ( const PythonWrapperException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<int, int>( "testStr", 182, std::pair<std::string, float>( "false signature", 4.f ) );
		CHECK( false );
	}
	catch ( const PythonWrapperException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<std::pair<int, int>>( "testStr", "testStrWithParam" );
		CHECK( false );
	}
	catch ( const pybind11::cast_error & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		PythonBinding::Wrapper::Object pythonObj
			= customModule.runFunction<PythonBinding::Wrapper::Object>( "generatePythonTestClass" );

		pythonObj.call( "print" );

		int value = pythonObj.call<int>( "get" );
		CHECK( value == 10 );

		pythonObj.call( "set", 5 );
		CHECK( pythonObj.call<int>( "get" ) == 5 );

		value = pythonObj.call<int, int>( "add", 7 );
		CHECK( value == 12 );
		CHECK( pythonObj.call<int>( "get" ) == 12 );

		CHECK( pythonObj.getMember<int>( "value" ) == 12 );

		pythonObj.setMember( "value", 33 );
		CHECK( pythonObj.getMember<int>( "value" ) == 33 );
	}
	catch ( PythonWrapperException & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}
};

TEST_CASE( "VTX_PYTHON_BINDING - Script execution ", "[python][binding][script]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	const FilePath internalDataDir = Util::Filesystem::getExecutableDir() / "data";
	const FilePath scriptPath	   = internalDataDir / "script_test.py";

	try
	{
		interpretor.runScript( scriptPath );
		CHECK( true );
	}
	catch ( const CommandException & e )
	{
		CHECK( false );
		VTX_ERROR( "{}", e.what() );
	}
	catch ( const std::exception & e )
	{
		CHECK( false );
		VTX_ERROR( "{}", e.what() );
	}

	const FilePath badScriptPath = internalDataDir / "bad_script_test.py";

	try
	{
		interpretor.runScript( badScriptPath );
		CHECK( false );
	}
	catch ( const ScriptException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	std::stringstream ssCommandRun = std::stringstream();
	ssCommandRun << "runScript(" << scriptPath << " )";

	try
	{
		interpretor.runCommand( ssCommandRun.str() );
		CHECK( true );
	}
	catch ( const ScriptException & e )
	{
		CHECK( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}
}
