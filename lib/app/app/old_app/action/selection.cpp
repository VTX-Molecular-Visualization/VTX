#include "selection.hpp"
#include "old_app/event/event.hpp"
#include "old_app/event/event_manager.hpp"
#include "old_app/model/representation/instantiated_representation.hpp"
#include "old_app/model/representation/representation_library.hpp"
#include "old_app/mvc/mvc_manager.hpp"
#include "old_app/object3d/scene.hpp"
#include "old_app/representation/representation_manager.hpp"
#include "old_app/selection/selection_manager.hpp"
#include "old_app/util/label.hpp"
#include "old_app/util/molecule.hpp"
#include "old_app/vtx_app.hpp"
#include <util/chrono.hpp>

namespace VTX::Action::Selection
{
	void SelectAll::execute()
	{
		const Object3D::Scene::MapMoleculePtrFloat & sceneMolecules = VTXApp::get().getScene().getMolecules();
		std::vector<Model::Molecule *>				 molecules		= std::vector<Model::Molecule *>();
		molecules.reserve( sceneMolecules.size() );

		for ( const std::pair<Model::Molecule * const, float> & moleculePair : sceneMolecules )
		{
			molecules.emplace_back( moleculePair.first );
		}

		const Object3D::Scene::VectorPathPtr & paths		  = VTXApp::get().getScene().getPaths();
		size_t								   viewpointCount = 0;
		for ( const Model::Path * const path : paths )
			viewpointCount += path->getViewpoints().size();

		std::vector<Model::Viewpoint *> viewpoints = std::vector<Model::Viewpoint *>();
		viewpoints.reserve( viewpointCount );
		for ( Model::Path * const path : paths )
		{
			for ( Model::Viewpoint * const viewpoint : path->getViewpoints() )
			{
				viewpoints.emplace_back( viewpoint );
			}
		}

		const Object3D::Scene::VectorMeshTrianglePtr & meshes = VTXApp::get().getScene().getMeshes();
		const Object3D::Scene::VectorLabelPtr &		   labels = VTXApp::get().getScene().getLabels();

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecules( molecules, false );
		VTX::Selection::SelectionManager::get().getSelectionModel().selectModels( viewpoints, true );
		VTX::Selection::SelectionManager::get().getSelectionModel().selectModels( meshes, true );
		VTX::Selection::SelectionManager::get().getSelectionModel().selectModels( labels, true );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void ChangeVisibility::show( const bool p_show )
	{
		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

			if ( _selection.isMoleculeFullySelected( molecule ) )
			{
				Util::Molecule::show( molecule, p_show, false, true );
			}
			else
			{
				molecule.setVisible( true );

				for ( const Model::Selection::PairChainIds & chainIds : molIds.second )
				{
					Model::Chain & chain = *molecule.getChain( chainIds.first );
					if ( _selection.isChainFullySelected( chain ) )
					{
						Util::Molecule::show( chain, p_show, false, false, false );
					}
					else
					{
						chain.setVisible( true );
						for ( const Model::Selection::PairResidueIds & residueIds : chainIds.second )
						{
							Model::Residue & residue = *molecule.getResidue( residueIds.first );
							if ( _selection.isResidueFullySelected( residue ) )
							{
								Util::Molecule::show( residue, p_show, false, false, false );
							}
							else
							{
								residue.setVisible( true );
								for ( const uint atomId : residueIds.second )
								{
									Model::Atom * const atom = molecule.getAtom( atomId );
									atom->setVisible( p_show, false );
								}
							}
						}
					}
				}
			}

			for ( Model::Category * const category : molecule.getCategories() )
				category->updateVisibilityState();

			molecule.notifyVisibilityChange();
			molecule.refreshVisibilities();
			molecule.computeRepresentationTargets();
		}
	}

	void ChangeVisibility::solo()
	{
		const Object3D::Scene::MapMoleculePtrFloat & moleculesInScene	  = VTXApp::get().getScene().getMolecules();
		const Model::Selection::MapMoleculeIds &	 moleculesInSelection = _selection.getMoleculesMap();

		for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule : moleculesInScene )
		{
			Model::Molecule * const							 molecule = sceneMolecule.first;
			Model::Selection::MapMoleculeIds::const_iterator itSelection
				= moleculesInSelection.find( molecule->getId() );

			if ( itSelection != moleculesInSelection.end() )
			{
				molecule->setVisible( true, false );

				Model::Selection::MapChainIds::const_iterator itChainSelection = itSelection->second.cbegin();

				for ( uint iChain = 0; iChain < molecule->getChainCount(); iChain++ )
				{
					Model::Chain * const chain = molecule->getChain( iChain );

					if ( chain == nullptr )
						continue;

					if ( itChainSelection != itSelection->second.cend() && iChain == itChainSelection->first )
					{
						chain->setVisible( true, false );

						Model::Selection::MapResidueIds::const_iterator itResidueSelection
							= itChainSelection->second.cbegin();

						for ( uint iResidue = chain->getIndexFirstResidue(); iResidue <= chain->getIndexLastResidue();
							  iResidue++ )
						{
							Model::Residue * const residue = molecule->getResidue( iResidue );

							if ( residue == nullptr )
								continue;

							if ( itResidueSelection != itChainSelection->second.cend()
								 && iResidue == itResidueSelection->first )
							{
								residue->setVisible( true, false );

								Model::Selection::VecAtomIds::const_iterator itAtomSelection
									= itResidueSelection->second.cbegin();

								for ( uint iAtom = residue->getIndexFirstAtom();
									  iAtom < residue->getIndexFirstAtom() + residue->getAtomCount();
									  iAtom++ )
								{
									Model::Atom * const atom = molecule->getAtom( iAtom );

									if ( atom == nullptr )
										continue;

									if ( itAtomSelection != itResidueSelection->second.cend()
										 && iAtom == *itAtomSelection )
									{
										atom->setVisible( true, false );
										itAtomSelection++;
									}
									else
									{
										atom->setVisible( false, false );
									}
								}

								itResidueSelection++;
							}
							else
							{
								residue->setVisible( false, false );
							}
						}
						itChainSelection++;
					}
					else
					{
						chain->setVisible( false, false );
					}
				}
			}
			else
			{
				molecule->setVisible( false, false );
			}

			for ( Model::Category * const category : molecule->getCategories() )
				category->updateVisibilityState();

			molecule->notifyVisibilityChange();
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

	void ChangeRepresentationPreset::execute()
	{
		Model::Representation::Representation * const preset
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		Representation::RepresentationManager::get().instantiateRepresentations( preset, _selection );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Extract::execute()
	{
		Model::Selection * const tmpSelection = MVC::MvcManager::get().instantiateModel<Model::Selection>();
		_selection.moveDataTo( *tmpSelection );
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		std::vector<Model::Molecule *> generatedMolecules = std::vector<Model::Molecule *>();
		generatedMolecules.reserve( tmpSelection->getMoleculesMap().size() );

		for ( const Model::Selection::PairMoleculeIds & moleculeSelectionData : tmpSelection->getMoleculesMap() )
		{
			const Model::ID & idMolSource = moleculeSelectionData.first;
			Model::Molecule & molecule	  = MVC::MvcManager::get().getModel<Model::Molecule>( idMolSource );

			if ( tmpSelection->isMoleculeFullySelected( molecule ) )
				continue;

			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractFromSelection( *tmpSelection, idMolSource );
			VTXApp::get().getScene().addMolecule( generatedMolecule );
			generatedMolecules.emplace_back( generatedMolecule );
		}

		MVC::MvcManager::get().deleteModel( tmpSelection );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecules( generatedMolecules );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::_copyFrame( const Model::Molecule & p_source, const Model::Selection & p_selection, const int p_frame )
	{
		Model::GeneratedMolecule * generatedMolecule
			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->copyFromSelection( _selection, p_source.getId(), p_frame );

		Util::Chrono chrono;

		chrono.start();

		generatedMolecule->applyTransform( p_source.getTransform() );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		chrono.stop();
		VTX_DEBUG( "Molecule " + generatedMolecule->getDisplayName() + " copied in " + chrono.elapsedTimeStr() );
	}

} // namespace VTX::Action::Selection
