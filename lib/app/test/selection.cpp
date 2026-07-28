#include "app/action/action_manager.hpp"
#include "app/services.hpp"
#include <app/action/selection.hpp>
#include <app/action/visibility.hpp>
#include <app/fixture.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <optional>
#include <vector>

namespace
{
	using namespace VTX;
	using namespace Core::Struct;
	using namespace App;
	using namespace App::Action::Selection;
	using namespace App::Action::Visibility;
	using namespace App::Helper::System;
	using namespace App::System;

	void _checkAtomRangeSelection(
		const VTX::Util::Math::BitSet & p_selection,
		const IndexRange &				p_range,
		const bool						p_selected
	)
	{
		for ( const VTX::Index atom : p_range )
		{
			CHECK( p_selection.test( atom ) == p_selected );
		}
	}

	std::optional<VTX::Index> _findResidueWithMinimumAtomCount(
		const Topology & p_topology,
		const VTX::Index p_minCount
	)
	{
		for ( VTX::Index residue = 0; residue < p_topology.getResidueCount(); ++residue )
		{
			if ( p_topology.getResidueAtomCount( residue ) >= p_minCount )
			{
				return residue;
			}
		}

		return std::nullopt;
	}
} // namespace

TEST_CASE( "VTX_APP - Selection - Extend selection to 4 angstrom", "[extendSelection][selection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "1AGA.mmtf" ) );
	Entity		 system1	= getSystemByFileName( "1AGA.mmtf" );
	const auto & selection1 = App::REG().get<App::System::Selection>( system1 );

	// Testing extendSelectionFromSelec
	Index toMask { 120 };
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, toMask, false );

	const Index toSelect { 125 };
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system1, toSelect );

	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "2qwo.pdb" ) );
	App::ACTION().execute<App::Action::Selection::ExtendSelection>( 4.0 );

	int count { 0 };
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
		{
			count++;
		}
	}
	CHECK( count == 13 );
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, toMask );

	App::ACTION().execute<Clear>();

	// Testing extendSelectionFromNonSelec
	Entity					 system2	= getSystemByFileName( "2qwo.pdb" );
	const auto &			 selection2 = App::REG().get<App::System::Selection>( system2 );
	Core::Struct::IndexRange range { 1, selection2.atoms.size() };
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system2, range );

	toMask = 46;
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, toMask, false );
	App::ACTION().execute<App::Action::Selection::ExtendSelection>( 5.0 );

	count = 0;
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
		{
			count++;
		}
	}
	CHECK( count == 4171 );

	App::ACTION().execute<Clear>();

	range = { 1, selection1.atoms.size() };
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system1, range );
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, range, false );
	App::ACTION().execute<App::Action::Selection::ExtendSelection>( 4.0 );

	count = 0;
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
		{
			count++;
		}
	}
	CHECK( count == 0 );
}

TEST_CASE( "VTX_APP - Selection - Extend selection to residues", "[extendSelectionResidue][selection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "1AGA.mmtf" ) );
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "2qwo.pdb" ) );
	Entity system1 = getSystemByFileName( "1AGA.mmtf" );
	Entity system2 = getSystemByFileName( "2qwo.pdb" );

	Index toSelect { 118 };
	Index toMask { 119 };
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system1, toSelect );
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, toMask, false );

	toSelect = 1802;
	toMask	 = 1799;
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system2, toSelect );
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system2, toMask, false );

	App::ACTION().execute<App::Action::Selection::ExtendSelectionRes>();

	int count { 0 };
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
		{
			count++;
		}
	}
	CHECK( count == 19 );
}

TEST_CASE( "VTX_APP - Selection - RevertSelection", "[revertSelection][selection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "1AGA.mmtf" ) );
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "2qwo.pdb" ) );
	Entity		 system1	= getSystemByFileName( "1AGA.mmtf" );
	Entity		 system2	= getSystemByFileName( "2qwo.pdb" );
	const auto & selection1 = App::REG().get<App::System::Selection>( system1 );

	Core::Struct::IndexRange range { 1, selection1.atoms.size() - 10 };
	App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system1, range );

	Index toMask { 1 };
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system2, toMask, false );

	toMask = selection1.atoms.size() - 1;
	App::ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system1, toMask, false );

	App::ACTION().execute<App::Action::Selection::RevertSelection>();

	int count { 0 };
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
		{
			count++;
		}
	}
	CHECK( count == 4153 );
}

TEST_CASE( "VTX_APP - Selection - Loaded system starts empty", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ ent, topology, selection ] = App::ECS::getFirstEntityWithComponents<Topology, System::Selection>();

	REQUIRE( topology.getChainCount() >= 1 );
	REQUIRE( topology.getResidueCount() >= 1 );
	REQUIRE( topology.getAtomCount() >= 1 );

	CHECK( selection.atoms.none() );
	CHECK( selection.atoms.count() == 0 );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::CHAIN, 0 } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::ATOM, 0 } ) == E_SELECTION_STATE::NONE );
}

TEST_CASE( "VTX_APP - Selection - SetSelected maps items to atom selections", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ system, topology, selection ] = App::ECS::getFirstEntityWithComponents<Topology, System::Selection>();

	SECTION( "atom overloads support replace, append and subtract" )
	{
		const auto residueIndexOpt = _findResidueWithMinimumAtomCount( topology, 2 );
		REQUIRE( residueIndexOpt.has_value() );

		const Index		 residueIndex = *residueIndexOpt;
		const IndexRange residueAtoms = topology.getResidueAtomRange( residueIndex );
		const Index		 firstAtom	  = residueAtoms.getFirst();
		const Index		 secondAtom	  = firstAtom + 1;

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, firstAtom );

		CHECK( REG().get<System::Selection>( system ).atoms.count() == 1 );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::ATOM, firstAtom } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, residueIndex } ) == E_SELECTION_STATE::PARTIAL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::PARTIAL );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, std::vector<Index> { secondAtom }, true, true );

		const auto & updatedSelection = REG().get<System::Selection>( system );
		CHECK( updatedSelection.atoms.test( firstAtom ) );
		CHECK( updatedSelection.atoms.test( secondAtom ) );
		CHECK( updatedSelection.atoms.count() == 2 );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, firstAtom, false, true );

		CHECK_FALSE( REG().get<System::Selection>( system ).atoms.test( firstAtom ) );
		CHECK( REG().get<System::Selection>( system ).atoms.test( secondAtom ) );
		CHECK( REG().get<System::Selection>( system ).atoms.count() == 1 );
	}

	SECTION( "residue range overload expands residue indexes into atom ranges" )
	{
		REQUIRE( topology.getResidueCount() >= 3 );

		const IndexRange selectedResidues( 0, 2 );
		const IndexRange firstResidueAtoms	= topology.getResidueAtomRange( 0 );
		const IndexRange secondResidueAtoms = topology.getResidueAtomRange( 1 );
		const IndexRange thirdResidueAtoms	= topology.getResidueAtomRange( 2 );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::RESIDUE>>( system, selectedResidues );

		const auto & updatedSelection = REG().get<System::Selection>( system );
		_checkAtomRangeSelection( updatedSelection.atoms, firstResidueAtoms, true );
		_checkAtomRangeSelection( updatedSelection.atoms, secondResidueAtoms, true );
		_checkAtomRangeSelection( updatedSelection.atoms, thirdResidueAtoms, false );

		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 1 } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 2 } ) == E_SELECTION_STATE::NONE );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::PARTIAL );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::RESIDUE>>( system, Index( 0 ), false, true );

		_checkAtomRangeSelection( REG().get<System::Selection>( system ).atoms, firstResidueAtoms, false );
		_checkAtomRangeSelection( REG().get<System::Selection>( system ).atoms, secondResidueAtoms, true );
	}

	SECTION( "system selection uses the default empty range list as full-system selection" )
	{
		const IndexRangeList noRanges;
		ACTION().execute<SetSelected<E_SYSTEM_ITEM::SYSTEM>>( system, noRanges, true );

		CHECK( REG().get<System::Selection>( system ).atoms.count() == topology.getAtomCount() );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::FULL );
	}
}

TEST_CASE( "VTX_APP - Selection - Clear handles this system, others and all systems", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	Entity	firstSystem	 = InvalidEntity;
	Entity	secondSystem = InvalidEntity;
	app.loadSystem();
	app.loadSystem();

	Registry & reg = App::REG();

	// Get the two loaded entities from view.
	{
		uint systemCount = 0;
		for ( const auto entity : reg.view<Topology>() )
		{
			if ( systemCount == 0 )
			{
				firstSystem = entity;
			}
			else if ( systemCount == 1 )
			{
				secondSystem = entity;
			}
			else
			{
				FAIL( "More than 2 systems loaded, test setup failure" );
			}
			systemCount++;
		}
		REQUIRE( systemCount == 2 );
		REQUIRE( firstSystem != InvalidEntity );
		REQUIRE( secondSystem != InvalidEntity );
	}

	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( firstSystem, Index( 0 ) );
	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( secondSystem, Index( 0 ) );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.count() == 1 );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.count() == 1 );

	ACTION().execute<Clear>( firstSystem, Clear::E_MODE::THIS );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.none() );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.count() == 1 );

	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( firstSystem, Index( 0 ) );
	ACTION().execute<Clear>( firstSystem, Clear::E_MODE::OTHERS );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.count() == 1 );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.none() );

	ACTION().execute<Clear>();

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.none() );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.none() );
}
