#include "analysis.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX::Util::Analysis
{
	void pickTargetAndComparersFromSelection( const Model::Selection &		   p_selection,
											  const Model::Molecule *&		   p_target,
											  std::vector<Model::Molecule *> & p_comparers )
	{
		Model::Selection::MapMoleculeIds::const_iterator it = p_selection.getMoleculesMap().begin();

		std::list<Model::Molecule *> sortedMolecules = std::list<Model::Molecule *>();

		for ( const Model::Selection::PairMoleculeIds & pairMoleculeID : p_selection.getMoleculesMap() )
		{
			Model::Molecule * const currentMolecule
				= &MVC::MvcManager::get().getModel<Model::Molecule>( pairMoleculeID.first );

			const int currentMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( *currentMolecule );

			bool inserted = false;

			for ( std::list<Model::Molecule *>::const_iterator it = sortedMolecules.begin();
				  it != sortedMolecules.end();
				  it++ )
			{
				const int sortedMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( **it );

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
		p_comparers = std::vector<Model::Molecule *>();
		p_comparers.resize( sortedMolecules.size() - 1 );

		std::move( ++sortedMolecules.begin(), sortedMolecules.end(), p_comparers.begin() );
	}

	void pickTargetAndComparersFromSelection( const Model::Selection &				 p_selection,
											  const Model::Molecule *&				 p_target,
											  std::vector<const Model::Molecule *> & p_comparers )
	{
		Model::Selection::MapMoleculeIds::const_iterator it = p_selection.getMoleculesMap().begin();

		std::list<Model::Molecule *> sortedMolecules = std::list<Model::Molecule *>();

		for ( const Model::Selection::PairMoleculeIds & pairMoleculeID : p_selection.getMoleculesMap() )
		{
			Model::Molecule * const currentMolecule
				= &MVC::MvcManager::get().getModel<Model::Molecule>( pairMoleculeID.first );

			const int currentMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( *currentMolecule );

			bool inserted = false;

			for ( std::list<Model::Molecule *>::const_iterator it = sortedMolecules.begin();
				  it != sortedMolecules.end();
				  it++ )
			{
				const int sortedMoleculeSceneIndex = VTXApp::get().getScene().getItemPosition( **it );

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
		p_comparers = std::vector<const Model::Molecule *>();
		p_comparers.resize( sortedMolecules.size() - 1 );

		std::move( ++sortedMolecules.begin(), sortedMolecules.end(), p_comparers.begin() );
	}

} // namespace VTX::Util::Analysis
