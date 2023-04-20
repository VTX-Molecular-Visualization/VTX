#include "app/action/selection.hpp"
#include "app/old_app/model/label.hpp"
#include "app/old_app/model/path.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/representation/representation_library.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/label.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
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

	void SelectModels::execute()
	{
		std::vector<Model::Molecule *> molecules  = std::vector<Model::Molecule *>();
		std::vector<Model::Category *> categories = std::vector<Model::Category *>();
		std::vector<Model::Chain *>	   chains	  = std::vector<Model::Chain *>();
		std::vector<Model::Residue *>  residues	  = std::vector<Model::Residue *>();
		std::vector<Model::Atom *>	   atoms	  = std::vector<Model::Atom *>();

		std::vector<Model::Path *>		paths	   = std::vector<Model::Path *>();
		std::vector<Model::Viewpoint *> viewpoints = std::vector<Model::Viewpoint *>();
		std::vector<Model::Label *>		labels	   = std::vector<Model::Label *>();

		for ( const Model::ID modelId : _models )
		{
			ID::VTX_ID modelTypeId = VTX::Core::MVC::MvcManager::get().getModelTypeID( modelId );

			if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
			{
				Model::Molecule & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_CATEGORY )
			{
				Model::Category & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
			{
				Model::Chain & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
			{
				Model::Residue & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
			{
				Model::Atom & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Atom>( modelId );
				atoms.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_PATH )
			{
				Model::Path & path = VTX::Core::MVC::MvcManager::get().getModel<Model::Path>( modelId );
				paths.emplace_back( &path );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Viewpoint>( modelId );
				viewpoints.emplace_back( &model );
			}
			else if ( Util::Label::isLabelType( modelTypeId ) )
			{
				Model::Label & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Label>( modelId );
				labels.emplace_back( &model );
			}
		}

		_selection.selectModels( molecules, categories, chains, residues, atoms, _appendToSelection );
		_selection.selectModels( paths, true );
		_selection.selectModels( viewpoints, true );
		_selection.selectModels( labels, true );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectModels::execute()
	{
		std::vector<Model::Molecule *> molecules  = std::vector<Model::Molecule *>();
		std::vector<Model::Category *> categories = std::vector<Model::Category *>();
		std::vector<Model::Chain *>	   chains	  = std::vector<Model::Chain *>();
		std::vector<Model::Residue *>  residues	  = std::vector<Model::Residue *>();
		std::vector<Model::Atom *>	   atoms	  = std::vector<Model::Atom *>();

		for ( const Model::ID modelId : _models )
		{
			ID::VTX_ID modelTypeId = VTX::Core::MVC::MvcManager::get().getModelTypeID( modelId );

			if ( modelTypeId == VTX::ID::Model::MODEL_MOLECULE )
			{
				Model::Molecule & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_CATEGORY )
			{
				Model::Category & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_CHAIN )
			{
				Model::Chain & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_RESIDUE )
			{
				Model::Residue & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == VTX::ID::Model::MODEL_ATOM )
			{
				Model::Atom & model = VTX::Core::MVC::MvcManager::get().getModel<Model::Atom>( modelId );
				atoms.emplace_back( &model );
			}
		}

		_selection.unselectModels( molecules, categories, chains, residues, atoms );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void SelectMolecule::execute()
	{
		_selection.selectMolecules( _molecules, _appendToSelection );
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void SelectCategory::execute()
	{
		_selection.selectCategories( _categories, _appendToSelection );
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void SelectChain::execute()
	{
		_selection.selectChains( _chains, _appendToSelection );
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void SelectResidue::execute()
	{
		_selection.selectResidues( _residues, _appendToSelection );
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void SelectAtom::execute()
	{
		_selection.selectAtoms( _atoms, _appendToSelection );
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectMolecule::execute()
	{
		if ( _check )
			_selection.unselectMolecules( _molecules );
		else
			_selection.unselectMoleculesWithCheck( _molecules );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectCategory::execute()
	{
		if ( _check )
			_selection.unselectCategories( _categories );
		else
			_selection.unselectCategoriesWithCheck( _categories );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectChain::execute()
	{
		if ( _check )
			_selection.unselectChains( _chains );
		else
			_selection.unselectChainsWithCheck( _chains );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectResidue::execute()
	{
		if ( _check )
			_selection.unselectResidues( _residues );
		else
			_selection.unselectResiduesWithCheck( _residues );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectAtom::execute()
	{
		if ( _check )
			_selection.unselectAtoms( _atoms );
		else
			_selection.unselectAtomsWithCheck( _atoms );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void ClearSelection::execute()
	{
		_selection.clear();
		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}
	void SelectViewpoint::execute() { _selection.selectModels( _viewpoints, _appendToSelection ); }

	void UnselectViewpoint::execute()
	{
		if ( _check )
			_selection.unselectModels( _viewpoints );
		else
			_selection.unselectModelsWithCheck( _viewpoints );

		// VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		switch ( _mode )
		{
		case VISIBILITY_MODE::SHOW:
		case VISIBILITY_MODE::HIDE:
		case VISIBILITY_MODE::ALL: show( _getVisibilityBool() ); break;
		case VISIBILITY_MODE::SOLO: solo(); break;
		case VISIBILITY_MODE::TOGGLE: break;
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}
	void ChangeVisibility::show( const bool p_show )
	{
		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

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

	void ToggleWatersVisibility::execute()
	{
		bool showWater = true;

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			showWater				   = showWater && !molecule.showWater();
		}

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			molecule.setShowWater( showWater );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleSolventVisibility::execute()
	{
		bool showSolvent = true;

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			showSolvent				   = showSolvent && !molecule.showSolvent();
		}

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			molecule.setShowSolvent( showSolvent );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleHydrogensVisibility::execute()
	{
		bool showHydrogen = true;

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			showHydrogen			   = showHydrogen && !molecule.showHydrogen();
		}

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			molecule.setShowHydrogen( showHydrogen );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleIonsVisibility::execute()
	{
		bool showIons = true;

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			showIons				   = showIons && !molecule.showIon();
		}

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			molecule.setShowIon( showIons );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleTrajectoryPlaying::execute()
	{
		bool play = true;

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
			if ( molecule.hasTrajectory() )
				play = play && !molecule.isPlaying();
		}

		for ( const Model::Selection::PairMoleculeIds & molIds : _selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

			if ( molecule.hasTrajectory() )
			{
				if ( molecule.isAtEndOfTrajectoryPlay() && play )
					molecule.resetTrajectoryPlay();

				molecule.setIsPlaying( play );
			}
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		Model::Representation::Representation * const preset
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		Representation::RepresentationManager::get().instantiateRepresentations( preset, _selection );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		for ( const Model::ID & selectedObjectID : _selection.getItems() )
		{
			const ID::VTX_ID & modelTypeID = VTX::Core::MVC::MvcManager::get().getModelTypeID( selectedObjectID );

			if ( modelTypeID == VTX::ID::Model::MODEL_MOLECULE )
			{
				const Model::Molecule & source	 = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( selectedObjectID );
				const int				nbFrames = source.getFrameCount();

				if ( _frame == Model::GeneratedMolecule::ALL_FRAMES_SEPARATED_INDEX )
				{
					for ( int iFrame = 0; iFrame < nbFrames; iFrame++ )
					{
						_copyFrame( source, _selection, iFrame );
					}
				}
				// Duplicate only if the molecule has multiple frame and if the frame is under the molecule frame
				// count (test for multiple selection)
				else if ( nbFrames > 0 && _frame < nbFrames )
				{
					_copyFrame( source, _selection, _frame );
				}
			}
		}
	}
	void Copy::_copyFrame( const Model::Molecule & p_source, const Model::Selection & p_selection, const int p_frame )
	{
		Model::GeneratedMolecule * generatedMolecule
			= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->copyFromSelection( _selection, p_source.getId(), p_frame );

		Util::Chrono chrono;

		chrono.start();

		generatedMolecule->applyTransform( p_source.getTransform() );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		chrono.stop();
		VTX_DEBUG( "Molecule " + generatedMolecule->getDisplayName() + " copied in " + chrono.elapsedTimeStr() );
	}

	void Extract::execute()
	{
		Model::Selection * const tmpSelection = VTX::Core::MVC::MvcManager::get().instantiateModel<Model::Selection>();
		_selection.moveDataTo( *tmpSelection );
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		std::vector<Model::Molecule *> generatedMolecules = std::vector<Model::Molecule *>();
		generatedMolecules.reserve( tmpSelection->getMoleculesMap().size() );

		for ( const Model::Selection::PairMoleculeIds & moleculeSelectionData : tmpSelection->getMoleculesMap() )
		{
			const Model::ID & idMolSource = moleculeSelectionData.first;
			Model::Molecule & molecule	  = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( idMolSource );

			if ( tmpSelection->isMoleculeFullySelected( molecule ) )
				continue;

			Model::GeneratedMolecule * const generatedMolecule
				= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractFromSelection( *tmpSelection, idMolSource );
			VTXApp::get().getScene().addMolecule( generatedMolecule );
			generatedMolecules.emplace_back( generatedMolecule );
		}

		VTX::Core::MVC::MvcManager::get().deleteModel( tmpSelection );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecules( generatedMolecules );

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		std::vector<Model::Molecule *> moleculesToDelete = std::vector<Model::Molecule *>();

		const std::set<Model::ID>		 itemsToDeleteCopy		 = _selection.getItems();
		Model::Selection::MapMoleculeIds moleculeMapToDeleteCopy = _selection.getMoleculesMap();
		_selection.clear();

		for ( const Model::ID & selectedObjectID : itemsToDeleteCopy )
		{
			const ID::VTX_ID & modelTypeID = VTX::Core::MVC::MvcManager::get().getModelTypeID( selectedObjectID );

			if ( modelTypeID == VTX::ID::Model::MODEL_MOLECULE )
			{
				const Model::Selection::PairMoleculeIds & molIds = *moleculeMapToDeleteCopy.find( selectedObjectID );

				Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				if ( molIds.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					moleculesToDelete.emplace_back( &molecule );
					continue;
				}

				for ( const Model::Selection::PairChainIds & chainIds : molIds.second )
				{
					Model::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						molecule.removeChain( chain.getIndex(), true, true, false );
						continue;
					}

					for ( const Model::Selection::PairResidueIds & residueIds : chainIds.second )
					{
						Model::Residue & residue = *molecule.getResidue( residueIds.first );

						if ( residueIds.second.getFullySelectedChildCount() == residue.getRealAtomCount() )
						{
							molecule.removeResidue( residue.getIndex(), true, true, true, false );
							continue;
						}

						for ( const uint atomId : residueIds.second )
						{
							molecule.removeAtom( atomId, true, true, true, false );
						}
					}
				}

				if ( molecule.isEmpty() )
				{
					moleculesToDelete.emplace_back( &molecule );
				}
				else
				{
					// Call notify only once after all modif in molecule
					molecule.refreshStructure();
					molecule.computeAllRepresentationData();
					molecule.notifyStructureChange();
				}
			}
			else if ( modelTypeID == VTX::ID::Model::MODEL_PATH )
			{
				Model::Path & path = VTX::Core::MVC::MvcManager::get().getModel<Model::Path>( selectedObjectID );

				VTX::VTXApp::get().getScene().removePath( &path );
				VTX::Core::MVC::MvcManager::get().deleteModel( &path );
			}
			else if ( modelTypeID == VTX::ID::Model::MODEL_VIEWPOINT )
			{
				Model::Viewpoint & viewpoint = VTX::Core::MVC::MvcManager::get().getModel<Model::Viewpoint>( selectedObjectID );

				Model::Path * const path = viewpoint.getPathPtr();
				path->removeViewpoint( &viewpoint );
				VTX::Core::MVC::MvcManager::get().deleteModel( &viewpoint );

				path->refreshAllDurations();
			}
			else if ( Util::Label::isLabelType( modelTypeID ) )
			{
				Model::Label & label = VTX::Core::MVC::MvcManager::get().getModel<Model::Label>( selectedObjectID );
				VTXApp::get().getScene().removeLabel( &label );
				VTX::Core::MVC::MvcManager::get().deleteModel<Model::Label>( &label );
			}
		}

		for ( Model::Molecule * const moleculeToDelete : moleculesToDelete )
		{
			VTXApp::get().getScene().removeMolecule( moleculeToDelete );
			VTX::Core::MVC::MvcManager::get().deleteModel( moleculeToDelete );
		}

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::Action::Selection
