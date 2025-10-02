#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - System - Remove", "[unit][remove]" )
{
	using namespace VTX;
	using namespace VTX::App;
	using namespace VTX::App::Component::Chemistry;

	App::Fixture app;
	Test::Util::App::loadTestSystem();

	App::Core::ECS::BaseEntity molEntity	   = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	System &				   systemComponent = ECS_REGISTRY().getComponent<System>( molEntity );

	Index expectedRemovedAtoms = 0;
	bool  callbackValidated	   = 0;

	systemComponent.onAtomRemoved +=
		[ & ]( const IndexRangeList & p_atoms ) { callbackValidated = ( expectedRemovedAtoms == p_atoms.count() ); };

	// Remove first atom of residue
	{
		const Index			  atomIndex = systemComponent.getResidue( 0 )->getIndexFirstAtom();
		Atom * const		  atom		= systemComponent.getAtom( atomIndex );
		const Residue * const residue	= atom->getResiduePtr();

		expectedRemovedAtoms = 1;
		atom->remove();

		CHECK( callbackValidated );
		CHECK( systemComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !systemComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexFirstAtom() == ( atomIndex + 1 ) );
	}
	// Remove last atom of residue
	{
		const Index			  atomIndex = systemComponent.getResidue( 0 )->getIndexLastAtom();
		Atom * const		  atom		= systemComponent.getAtom( atomIndex );
		const Residue * const residue	= atom->getResiduePtr();

		atom->remove();

		CHECK( systemComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !systemComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexLastAtom() == ( atomIndex - 1 ) );
	}

	// Remove first residue
	{
		const Index			residueIndex	 = 0;
		Residue * const		residue			 = systemComponent.getResidue( residueIndex );
		const IndexRange	residueAtomRange = residue->getAtomRange();
		const Chain * const chain			 = residue->getChainPtr();

		expectedRemovedAtoms = residueAtomRange.getCount();
		residue->remove();

		CHECK( callbackValidated == true );
		CHECK( systemComponent.getResidue( residueIndex ) == nullptr );
		CHECK( !systemComponent.getActiveAtoms().contains( residueAtomRange ) );

		for ( Index i = residueAtomRange.getFirst(); i <= residueAtomRange.getLast(); i++ )
		{
			CHECK( systemComponent.getAtom( i ) == nullptr );
		}
		CHECK( chain->getIndexFirstResidue() == ( residueIndex + 1 ) );
	}

	// Remove last residue of chain
	{
		const Chain * const chain			 = systemComponent.getChain( 0 );
		const Index			residueIndex	 = chain->getIndexLastResidue();
		Residue * const		residue			 = systemComponent.getResidue( residueIndex );
		const IndexRange	residueAtomRange = residue->getAtomRange();

		residue->remove();

		CHECK( systemComponent.getResidue( residueIndex ) == nullptr );
		CHECK( !systemComponent.getActiveAtoms().contains( residueAtomRange ) );

		for ( Index i = residueAtomRange.getFirst(); i <= residueAtomRange.getLast(); i++ )
		{
			CHECK( systemComponent.getAtom( i ) == nullptr );
		}
		CHECK( chain->getIndexLastResidue() == ( residueIndex - 1 ) );
	}

	// Remove first chain
	{
		const Index		 chainIndex		   = 0;
		Chain * const	 chain			   = systemComponent.getChain( chainIndex );
		const IndexRange chainResidueRange = chain->getResidueRange();
		const IndexRange chainAtomRange	   = chain->getAtomRange();

		expectedRemovedAtoms = chainAtomRange.getCount();
		chain->remove();

		CHECK( callbackValidated == true );
		CHECK( systemComponent.getChain( chainIndex ) == nullptr );
		CHECK( !systemComponent.getActiveAtoms().contains( chainAtomRange ) );

		for ( Index i = chainAtomRange.getFirst(); i <= chainAtomRange.getLast(); i++ )
		{
			CHECK( systemComponent.getAtom( i ) == nullptr );
		}

		for ( Index i = chainResidueRange.getFirst(); i <= chainResidueRange.getLast(); i++ )
		{
			CHECK( systemComponent.getResidue( i ) == nullptr );
		}
	}

	// Remove last atom
	{
		const Index	 defaultAtomSize = Index( systemComponent.getAtoms().size() );
		const Index	 atomIndex		 = defaultAtomSize - 1;
		Atom * const atom			 = systemComponent.getAtom( atomIndex );

		atom->remove();

		CHECK( systemComponent.getAtoms().size() == ( defaultAtomSize - 1 ) );
	}

	// Remove last residue
	{
		const Index defaultResidueSize = Index( systemComponent.getResidues().size() );
		const Index defaultAtomSize	   = Index( systemComponent.getAtoms().size() );

		const Index residueIndex = defaultResidueSize - 1;
		Residue &	residue		 = *systemComponent.getResidue( residueIndex );

		const Index residueAtomCount = residue.getAtomCount();

		residue.remove();

		CHECK( systemComponent.getAtoms().size() == ( defaultAtomSize - residueAtomCount ) );
		CHECK( systemComponent.getResidues().size() == ( defaultResidueSize - 1 ) );
	}

	// Remove last chain
	{
		const Index defaultRealChainCount	= systemComponent.getRealChainCount();
		const Index defaultRealResidueCount = systemComponent.getRealResidueCount();
		const Index defaultRealAtomCount	= systemComponent.getRealAtomCount();

		const Index defaultChainSize   = Index( systemComponent.getChains().size() );
		const Index defaultResidueSize = Index( systemComponent.getResidues().size() );
		const Index defaultAtomSize	   = Index( systemComponent.getAtoms().size() );

		const Index chainIndex = defaultChainSize - 1;
		Chain &		chain	   = *systemComponent.getChain( chainIndex );

		const Index chainResidueCount = chain.getResidueCount();
		const Index chainAtomCount	  = chain.getIndexLastAtom() - chain.getIndexFirstAtom() + 1;

		chain.remove();

		CHECK( systemComponent.getChains().size() == ( defaultChainSize - 1 ) );
		CHECK( systemComponent.getResidues().size() == ( defaultResidueSize - chainResidueCount ) );
		CHECK( systemComponent.getAtoms().size() == ( defaultAtomSize - chainAtomCount ) );

		CHECK( systemComponent.getRealChainCount() == ( defaultRealChainCount - 1 ) );
		CHECK( systemComponent.getRealResidueCount() == ( defaultRealResidueCount - chainResidueCount ) );
		CHECK( systemComponent.getRealAtomCount() == ( defaultRealAtomCount - chainAtomCount ) );
	}
};

TEST_CASE( "VTX_APP - System - Visibility", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;
	using namespace VTX::App::Component::Chemistry;

	App::Fixture app;
	Test::Util::App::loadTestSystem();

	App::Core::ECS::BaseEntity molEntity	   = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	System &				   systemComponent = ECS_REGISTRY().getComponent<System>( molEntity );

	Index expectedModifiedAtomCount = 0;
	bool  callbackValidated			= 0;

	systemComponent.onVisibilityChange +=
		[ & ]( const IndexRangeList & p_atoms, const Component::Chemistry::VISIBILITY_APPLY_MODE p_applyMode )
	{ callbackValidated = ( expectedModifiedAtomCount == p_atoms.count() ); };

	// Atom visibility
	{
		const Index		atomIndex = 0;
		Atom &			atom	  = *( systemComponent.getAtom( atomIndex ) );
		const Residue & residue	  = *( atom.getConstResiduePtr() );

		expectedModifiedAtomCount = 1;

		atom.setVisible( false );
		CHECK( callbackValidated );
		CHECK( atom.isVisible() == false );
		CHECK( residue.isVisible() );

		atom.setVisible( true );
		CHECK( callbackValidated );
		CHECK( atom.isVisible() == true );
		CHECK( residue.isVisible() );
	}

	// Residues visibility
	{
		Residue &		 residue   = *( systemComponent.getResidue( 0 ) );
		const IndexRange atomRange = residue.getAtomRange();

		expectedModifiedAtomCount = atomRange.getCount();

		// From residue
		{
			residue.setVisible( false );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( residue.isVisible() == false );

			residue.setVisible( true );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( residue.isVisible() == true );
		}

		// From atoms
		{
			systemComponent.setVisible( atomRange, false );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( residue.isVisible() == false );

			systemComponent.setVisible( atomRange, true );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( residue.isVisible() == true );
		}
	}

	// Chains visibility
	{
		Chain &			 chain		  = *( systemComponent.getChain( 0 ) );
		Residue &		 residue	  = *( systemComponent.getResidue( 0 ) );
		const IndexRange atomRange	  = chain.getAtomRange();
		const IndexRange residueRange = chain.getResidueRange();

		expectedModifiedAtomCount = atomRange.getCount();

		// From chain
		{
			chain.setVisible( false );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			chain.setVisible( true );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}

		// From residues
		{
			for ( Residue & residue : chain.residues() )
				residue.setVisible( false );

			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			for ( Residue & residue : chain.residues() )
				residue.setVisible( true );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}

		// From atoms
		{
			systemComponent.setVisible( atomRange, false );

			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( systemComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			systemComponent.setVisible( atomRange, true );
			CHECK( callbackValidated );
			CHECK( systemComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( systemComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}
	}
};
