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
#include <python_binding/binding/vtx_api.hpp>
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

	struct MockSystem
	{
		size_t						 initializedChainCount	 = 0;
		MockChain *					 chain					 = nullptr;
		size_t						 initializedResidueCount = 0;
		MockResidue *				 residue				 = nullptr;
		size_t						 initializedAtomCount	 = 0;
		MockAtom *					 atom					 = nullptr;
		size_t						 initializedBondCount	 = 0;
		std::string					 name { "el Systemos" };
		std::string					 pdbIdCode { "el pdb Codos" };
		bool						 visible	  = true;
		bool						 fullyVisible = true;
		std::map<atom_index_t, bool> atomVisibility;
		std::vector<atom_index_t>	 atomRemoved;
		size_t						 realChainCount	  = 0;
		size_t						 realResidueCount = 0;
		size_t						 realAtomCount	  = 0;
		std::vector<MockChain>		 chains;
		std::vector<MockResidue>	 residues;
		std::vector<MockAtom>		 atoms;

		void			  initChains( const size_t p_chainCount ) { initializedChainCount = p_chainCount; }
		MockChain *		  getChain( const size_t p_index ) { return chain; }
		const MockChain * getChain( const size_t p_index ) const { return chain; }

		void				initResidues( const size_t p_residueCount ) { initializedResidueCount = p_residueCount; }
		MockResidue *		getResidue( const size_t p_index ) { return residue; }
		const MockResidue * getResidue( const size_t p_index ) const { return residue; }

		void			 initAtoms( const size_t p_atomCount ) { initializedAtomCount = p_atomCount; }
		MockAtom *		 getAtom( const atom_index_t p_index ) { return atom; }
		const MockAtom * getAtom( const atom_index_t p_index ) const { return atom; }

		void initBonds( const size_t p_bondCount ) { initializedBondCount = p_bondCount; }

		const std::string & getName() const { return name; }
		void				setName( const std::string & p_name ) { name = p_name; }

		const std::string & getPdbIdCode() const { return pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { pdbIdCode = p_pdbIdCode; }

		bool isVisible() const { return visible; }
		bool isFullyVisible() const { return fullyVisible; }

		void setVisible( const bool p_visible ) { visible = p_visible; }
		void setVisible( const atom_index_t & p_atomId, bool p_visible )
		{
			atomVisibility.emplace( p_atomId, p_visible );
		}

		void remove( const atom_index_t & p_atomIndex ) { atomRemoved.push_back( p_atomIndex ); }

		size_t getRealChainCount() const { return realChainCount; }
		size_t getRealResidueCount() const { return realResidueCount; }
		size_t getRealAtomCount() const { return realAtomCount; }

		std::vector<MockChain> &		 getChains() { return chains; }
		const std::vector<MockChain> &	 getChains() const { return chains; }
		std::vector<MockResidue> &		 getResidues() { return residues; }
		const std::vector<MockResidue> & getResidues() const { return residues; }
		std::vector<MockAtom> &			 getAtoms() { return atoms; }
		const std::vector<MockAtom> &	 getAtoms() const { return atoms; }
	};
	struct MockChain
	{
		size_t		 index = 0;
		std::string	 name { "el Chainos" };
		size_t		 indexFirstResidue = 0;
		size_t		 indexLastResidue  = 0;
		size_t		 residueCount	   = 0;
		atom_index_t indexFirstAtom	   = 0;
		atom_index_t indexLastAtom	   = 0;
		std::string	 chainId { "el identifios" };
		bool		 visible	  = true;
		bool		 fullyVisible = true;
		bool		 removed	  = false;

		size_t getIndex() const { return index; }
		void   setIndex( const size_t p_index ) { index = p_index; }

		const std::string & getName() const { return name; }
		void				setName( const std::string & p_name ) { name = p_name; }

		size_t getIndexFirstResidue() const { return indexFirstResidue; }
		size_t getIndexLastResidue() const { return indexLastResidue; }
		void   setIndexFirstResidue( const size_t p_residueIndex ) { indexFirstResidue = p_residueIndex; }

		size_t getResidueCount() const { return residueCount; }
		void   setResidueCount( const size_t p_residueCount ) { residueCount = p_residueCount; }

		atom_index_t getIndexFirstAtom() const { return indexFirstAtom; }
		atom_index_t getIndexLastAtom() const { return indexLastAtom; }

		const std::string & getOriginalChainID() const { return chainId; }
		void				setOriginalChainID( const std::string & p_chainId ) { chainId = p_chainId; }

		bool isVisible() const { return visible; }
		bool isFullyVisible() const { return fullyVisible; }

		void setVisible( const bool p_visible ) { visible = p_visible; }

		void remove() { removed = true; }
	};
	struct MockResidue
	{
		uint64_t	 index				  = 0;
		atom_index_t indexFirstAtom		  = 0;
		atom_index_t indexLastAtom		  = 0;
		uint64_t	 atomCount			  = 0;
		size_t		 indexFirstBond		  = 0;
		size_t		 bondCount			  = 0;
		size_t		 indexInOriginalChain = 0;
		std::string	 name { "el Residos" };
		bool		 visible	  = true;
		bool		 fullyVisible = true;
		MockChain *	 chain		  = nullptr;
		MockSystem * system		  = nullptr;

		size_t getIndex() const { return index; }
		void   setIndex( const size_t p_index ) { index = p_index; }

		atom_index_t getIndexFirstAtom() const { return indexFirstAtom; }
		void		 setIndexFirstAtom( const atom_index_t p_indexFirstAtom ) { indexFirstAtom = p_indexFirstAtom; }
		atom_index_t getIndexLastAtom() const { return indexLastAtom; }

		uint64_t getAtomCount() const { return atomCount; }
		void	 setAtomCount( const atom_index_t p_atomCount ) { atomCount = p_atomCount; }

		size_t getIndexFirstBond() const { return indexFirstBond; }
		void   setIndexFirstBond( const size_t p_indexFirstBond ) { indexFirstBond = p_indexFirstBond; }

		size_t getBondCount() const { return bondCount; }
		void   setBondCount( const size_t p_bondCount ) { bondCount = p_bondCount; }

		size_t getIndexInOriginalChain() const { return indexInOriginalChain; }
		void   setIndexInOriginalChain( const size_t p_index ) { indexInOriginalChain = p_index; }

		const std::string_view getShortName() const { return name; }
		const std::string_view getName() const { return name; }
		const std::string_view getLongName() const { return name; }

		void setVisible( const bool p_visible ) { visible = p_visible; }
		bool isVisible() const { return visible; }
		bool isFullyVisible() const { return fullyVisible; }

		const MockChain *  getConstChainPtr() const { return chain; }
		MockChain *		   getChainPtr() { return chain; }
		const MockSystem * getConstSystemPtr() const { return system; }
		MockSystem *	   getSystemPtr() { return system; }
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

		atom_index_t		getIndex() const { return index; }
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

TEST_CASE( "VTX_PYTHON_BINDING - VTX class binding - Atom", "[python][binding][api][class]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	pybind11::module_ * vtxModule = nullptr;
	interpretor.getPythonModule( &vtxModule );
	PythonBinding::Binding::applyVtxBinding( *vtxModule ); // Allows to return VTX types

	Test::MockAtom mockedAtom {};
	mockedAtom.symbol	= VTX::Core::ChemDB::Atom::SYMBOL::A_HE;
	mockedAtom.position = Vec3f( 5.f, 5.f, 5.f );
	mockedAtom.index	= 18;

	interpretor.getModule().api().getPythonModule( &vtxModule );
	vtxModule->def(
		"TEST_getSampleAtom",
		[ mockedAtom = &mockedAtom ]() { return PythonBinding::API::Atom( *mockedAtom ); },
		pybind11::return_value_policy::move
	);
	pybind11::exec( "from vtx_python_api.API import *" );

	// index
	auto returnedAtom = pybind11::eval( "TEST_getSampleAtom()" );
	CHECK( returnedAtom.attr( "getIndex" )().cast<uint64_t>() == mockedAtom.index );
	pybind11::exec( "TEST_getSampleAtom().setIndex(22)" );
	CHECK( mockedAtom.index == 22 );

	// Name
	pybind11::exec( "TEST_getSampleAtom().setName('perlimpimpin')" );
	CHECK( mockedAtom.name == "perlimpimpin" );
	CHECK( pybind11::eval( "TEST_getSampleAtom().getName()" ).cast<std::string>() == "perlimpimpin" );

	// Symbol
	CHECK(
		pybind11::eval( "TEST_getSampleAtom().getSymbol()" ).cast<VTX::Core::ChemDB::Atom::SYMBOL>()
		== mockedAtom.symbol
	);
	pybind11::exec( "TEST_getSampleAtom().setSymbol(ATOM_SYMBOL.A_V)" );
	CHECK( mockedAtom.symbol == VTX::Core::ChemDB::Atom::SYMBOL::A_V );

	// positions
	CHECK( pybind11::eval( "TEST_getSampleAtom().getLocalPosition()" ).cast<Vec3f>() == mockedAtom.getLocalPosition() );
	CHECK( pybind11::eval( "TEST_getSampleAtom().getWorldPosition()" ).cast<Vec3f>() == mockedAtom.getWorldPosition() );
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
