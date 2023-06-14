#include "tool/analysis/util.hpp"
#include "tool/analysis/rmsd/core/rmsd.hpp"
#include <app/old/application/scene.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/vtx_app.hpp>

namespace VTX::Tool::Analysis::Util
{
	void pickTargetAndComparersFromSelection( const App::Old::Application::Selection::SelectionModel &  p_selection,
											  const App::Old::Component::Chemistry::Molecule *&		   p_target,
											  std::vector<App::Old::Component::Chemistry::Molecule *> & p_comparers )
	{
		App::Old::Application::Selection::SelectionModel::MapMoleculeIds::const_iterator it
			= p_selection.getMoleculesMap().begin();

		std::list<App::Old::Component::Chemistry::Molecule *> sortedMolecules
			= std::list<App::Old::Component::Chemistry::Molecule *>();

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & pairMoleculeID :
			  p_selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule * const currentMolecule
				= &VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( pairMoleculeID.first );

			const int currentMoleculeSceneIndex = App::Old::VTXApp::get().getScene().getItemPosition( *currentMolecule );

			bool inserted = false;

			for ( std::list<App::Old::Component::Chemistry::Molecule *>::const_iterator it = sortedMolecules.begin();
				  it != sortedMolecules.end();
				  it++ )
			{
				const int sortedMoleculeSceneIndex = App::Old::VTXApp::get().getScene().getItemPosition( **it );

				if ( currentMoleculeSceneIndex < sortedMoleculeSceneIndex )
				{
					sortedMolecules.insert( it, currentMolecule );

					inserted = true;
					break;
				}
			}

			if ( !inserted )
			{
				sortedMolecules.push_back( currentMolecule );
			}
		}

		p_target	= *sortedMolecules.begin();
		p_comparers = std::vector<App::Old::Component::Chemistry::Molecule *>();
		p_comparers.resize( sortedMolecules.size() - 1 );

		std::move( ++sortedMolecules.begin(), sortedMolecules.end(), p_comparers.begin() );
	}

	void pickTargetAndComparersFromSelection( const App::Old::Application::Selection::SelectionModel &		 p_selection,
											  const App::Old::Component::Chemistry::Molecule *&				 p_target,
											  std::vector<const App::Old::Component::Chemistry::Molecule *> & p_comparers )
	{
		App::Old::Application::Selection::SelectionModel::MapMoleculeIds::const_iterator it
			= p_selection.getMoleculesMap().begin();

		std::list<App::Old::Component::Chemistry::Molecule *> sortedMolecules
			= std::list<App::Old::Component::Chemistry::Molecule *>();

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & pairMoleculeID :
			  p_selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule * const currentMolecule
				= &VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( pairMoleculeID.first );

			const int currentMoleculeSceneIndex = App::Old::VTXApp::get().getScene().getItemPosition( *currentMolecule );

			bool inserted = false;

			for ( std::list<App::Old::Component::Chemistry::Molecule *>::const_iterator it = sortedMolecules.begin();
				  it != sortedMolecules.end();
				  it++ )
			{
				const int sortedMoleculeSceneIndex = App::Old::VTXApp::get().getScene().getItemPosition( **it );

				if ( currentMoleculeSceneIndex < sortedMoleculeSceneIndex )
				{
					sortedMolecules.insert( it, currentMolecule );

					inserted = true;
					break;
				}
			}

			if ( !inserted )
			{
				sortedMolecules.push_back( currentMolecule );
			}
		}

		p_target	= *sortedMolecules.begin();
		p_comparers = std::vector<const App::Old::Component::Chemistry::Molecule *>();
		p_comparers.resize( sortedMolecules.size() - 1 );

		std::move( ++sortedMolecules.begin(), sortedMolecules.end(), p_comparers.begin() );
	}

	void getAtomPositions( const App::Old::Application::Selection::SelectionModel & p_selection,
						   const App::Old::Component::Chemistry::Molecule *		   p_target,
						   std::vector<Vec3f> &								   p_positions )
	{
		const App::Old::Component::Chemistry::Molecule::AtomPositionsFrame & atomPositionFrame
			= p_target->getCurrentAtomPositionFrame();
		const App::Old::Application::Selection::SelectionModel::MapChainIds & chainMap
			= p_selection.getMoleculesMap().at( p_target->getId() );

		p_positions.resize( p_target->getAtomCount() );
		size_t counter = 0;

		for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainPair : chainMap )
		{
			for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residuePair : chainPair.second )
			{
				for ( const App::Old::Core::Model::ID & atomIndex : residuePair.second )
				{
					p_positions[ counter ] = atomPositionFrame[ atomIndex ];
					counter++;
				}
			}
		}

		p_positions.resize( counter );
	}

	std::string getRMSDLog( const RMSD::Core::RMSDData & p_data )
	{
		std::string log = "RMSD between " + p_data.getFirstMolecule()->getDisplayName() + " and "
						  + p_data.getSecondMolecule()->getDisplayName();

		if ( p_data.hasResidueCount() )
		{
			log += " (over " + std::to_string( p_data.getResidueCount() ) + " residues)";
		}
		else if ( p_data.hasAtomCount() )
		{
			log += " (over " + std::to_string( p_data.getAtomCount() ) + " atoms)";
		}

		log += " : " + std::to_string( p_data.getRMSD() );

		return log;
	}

} // namespace VTX::Tool::Analysis::Util
