#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/fixture.hpp>
#include <app/selection/selection_manager.hpp>
#include <app/selection/system_data.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_APP - Selection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;
	try
	{
		VTX_INFO( "TEST VTX_APP - Selection" );

		using AssignmentType = Selection::AssignmentType;

		App::Fixture app;

		Test::Util::App::loadSystem( "8OIT.mmtf" );
		const Component::Chemistry::System & mol1 = SCENE().getComponentByName<Component::Chemistry::System>( "8OIT" );
		const Component::Scene::Selectable & selectableMol1
			= SCENE().getComponentByName<Component::Scene::Selectable>( "8OIT" );

		Test::Util::App::loadSystem( "1AGA.mmtf" );
		const Component::Chemistry::System & mol2 = SCENE().getComponentByName<Component::Chemistry::System>( "1AGA" );
		const Component::Scene::Selectable & selectableMol2
			= SCENE().getComponentByName<Component::Scene::Selectable>( "1AGA" );
		VTX_INFO( "1AGA.mmtf loaded" );

		CHECK( CURRENT_SELECTION().isEmpty() );

		Selection::SelectionData & molSelData1 = CURRENT_SELECTION().select( selectableMol1 );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( !CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( !CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol1 );

		Selection::SelectionData & molSelData2 = CURRENT_SELECTION().select( mol2 );
		CHECK( !CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( !CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().select( selectableMol1, AssignmentType::APPEND );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol1 );

		CURRENT_SELECTION().clear();
		CHECK( CURRENT_SELECTION().isEmpty() );

		CURRENT_SELECTION().selectAll( { &selectableMol1, &selectableMol2 } );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().clear();

		std::vector<const Component::Scene::Selectable *> selectables = { &selectableMol1, &selectableMol2 };
		CURRENT_SELECTION().selectAll( selectables );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().clear();

		// std::vector<const Component::Chemistry::System *> systems = { &mol1, &mol2 };
		// CURRENT_SELECTION().selectAll( systems );
		// CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "Exception: {}", e.what() );
	}
}

TEST_CASE( "VTX_APP - Selection - Systems", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using SystemData	 = Selection::SystemData;
	using AssignmentType = Selection::AssignmentType;
	using IndexRange	 = Util::Math::Range<size_t>;
	using AtomIndexRange = Util::Math::Range<atom_index_t>;

	App::Fixture app;

	Test::Util::App::loadSystem( "1AGA.mmtf" );
	const Component::Chemistry::System & mol1 = SCENE().getComponentByName<Component::Chemistry::System>( "1AGA" );
	const Component::Scene::Selectable & selectableMol1
		= SCENE().getComponentByName<Component::Scene::Selectable>( "1AGA" );

	SystemData & molSelData1 = CURRENT_SELECTION().select<SystemData>( selectableMol1 );
	CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
	CHECK( molSelData1.isFullySelected() );
	CHECK( molSelData1.getCurrentObjectType() == SystemData::CurrentObjectTypeEnum::System );
	CHECK( &molSelData1.getCurrentObjectAsSystem() == &mol1 );

	molSelData1.clear();
	molSelData1.selectFullChain( *mol1.getChain( 0 ) );
	CHECK( !molSelData1.isFullySelected() );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( molSelData1.isResidueFullySelected( 0 ) );
	CHECK( molSelData1.isAtomSelected( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == SystemData::CurrentObjectTypeEnum::Chain );
	CHECK( &molSelData1.getCurrentObjectAsChain() == mol1.getChain( 0 ) );

	molSelData1.unselectAtom( *mol1.getAtom( 0 ) );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( !molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( !molSelData1.isResidueFullySelected( 0 ) );
	CHECK( !molSelData1.isAtomSelected( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == SystemData::CurrentObjectTypeEnum::Chain );
	CHECK( &molSelData1.getCurrentObjectAsChain() == mol1.getChain( 0 ) );

	molSelData1.selectAtom( *mol1.getAtom( 0 ) );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( molSelData1.isResidueFullySelected( 0 ) );
	CHECK( molSelData1.isAtomSelected( 0 ) );

	molSelData1.setCurrentObject( *mol1.getResidue( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == SystemData::CurrentObjectTypeEnum::Residue );
	CHECK( &molSelData1.getCurrentObjectAsResidue() == mol1.getResidue( 0 ) );

	molSelData1.unselectChain( *mol1.getChain( 0 ) );
	CHECK( !molSelData1.isChainSelected( 0 ) );
	CHECK( !molSelData1.isChainFullySelected( 0 ) );
	CHECK( !molSelData1.areResiduesSelected(
		IndexRange( mol1.getChain( 0 )->getIndexFirstResidue(), mol1.getChain( 0 )->getResidueCount() )
	) );
	CHECK( !molSelData1.areResiduesFullySelected(
		IndexRange( mol1.getChain( 0 )->getIndexFirstResidue(), mol1.getChain( 0 )->getResidueCount() )
	) );
	CHECK( !molSelData1.areAtomsSelected( AtomIndexRange::createFirstLast(
		mol1.getChain( 0 )->getIndexFirstAtom(), mol1.getChain( 0 )->getIndexLastAtom()
	) ) );

	CHECK( molSelData1.getCurrentObjectType() == SystemData::CurrentObjectTypeEnum::None );

	SystemData molData2 = SystemData( selectableMol1 );
	molSelData1.clear();
	molData2.clear();

	molData2.selectFullResidue( *mol1.getResidue( 0 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectAtom( *mol1.getAtom( 0 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectFullResidue( *mol1.getResidue( 0 ) );
	CHECK( molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectFullChain( *mol1.getChain( 0 ) );
	CHECK( molSelData1.contains( molData2 ) );
	CHECK( !molData2.contains( molSelData1 ) );

	molData2.selectFullChain( *mol1.getChain( 1 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( !molData2.contains( molSelData1 ) );
}

TEST_CASE( "VTX_APP - Selection - Benchmark", "[.][perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using AssignmentType = Selection::AssignmentType;

	App::Fixture app;

	Test::Util::App::loadSystem( "7Y7A.mmtf" );
	const Component::Chemistry::System & mol1 = SCENE().getComponentByName<Component::Chemistry::System>( "7Y7A" );
	const Component::Scene::Selectable & selectableMol1
		= SCENE().getComponentByName<Component::Scene::Selectable>( "7Y7A" );

	BENCHMARK( "Select full system" ) { CURRENT_SELECTION().select( selectableMol1 ); };

	BENCHMARK( "Select system worst case" )
	{
		Selection::SystemData & molData = CURRENT_SELECTION().select<Selection::SystemData>( selectableMol1 );

		const atom_index_t atomCount = atom_index_t( mol1.getAtoms().size() );
		for ( atom_index_t i = 0; i < atomCount; i += 2 )
		{
			molData.selectAtom( *mol1.getAtom( i ) );
		}
	};
}
