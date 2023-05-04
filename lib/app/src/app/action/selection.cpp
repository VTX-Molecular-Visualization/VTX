#include "app/action/selection.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/application/representation/representation_preset.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/component/object3d/label.hpp"
#include "app/component/video/path.hpp"
#include "app/mvc.hpp"
#include "app/vtx_app.hpp"
#include "app/util/label.hpp"
#include "app/util/molecule.hpp"
#include <util/chrono.hpp>

namespace VTX::App::Action::Selection
{
	void SelectAll::execute()
	{
		const App::Application::Scene::MapMoleculePtrFloat & sceneMolecules = App::VTXApp::get().getScene().getMolecules();
		std::vector<App::Component::Chemistry::Molecule *>	 molecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		molecules.reserve( sceneMolecules.size() );

		for ( const std::pair<App::Component::Chemistry::Molecule * const, float> & moleculePair : sceneMolecules )
		{
			molecules.emplace_back( moleculePair.first );
		}

		const App::Application::Scene::VectorPathPtr & paths		  = App::VTXApp::get().getScene().getPaths();
		size_t										   viewpointCount = 0;
		for ( const App::Component::Video::Path * const path : paths )
			viewpointCount += path->getViewpoints().size();

		std::vector<App::Component::Object3D::Viewpoint *> viewpoints
			= std::vector<App::Component::Object3D::Viewpoint *>();
		viewpoints.reserve( viewpointCount );
		for ( App::Component::Video::Path * const path : paths )
		{
			for ( App::Component::Object3D::Viewpoint * const viewpoint : path->getViewpoints() )
			{
				viewpoints.emplace_back( viewpoint );
			}
		}

		const App::Application::Scene::VectorMeshTrianglePtr & meshes = App::VTXApp::get().getScene().getMeshes();
		const App::Application::Scene::VectorLabelPtr &		   labels = App::VTXApp::get().getScene().getLabels();

		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecules( molecules,
																									   false );
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( viewpoints, true );
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( meshes, true );
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( labels, true );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectModels::execute()
	{
		std::vector<App::Component::Chemistry::Molecule *> molecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		std::vector<App::Component::Chemistry::Category *> categories
			= std::vector<App::Component::Chemistry::Category *>();
		std::vector<App::Component::Chemistry::Chain *>	  chains = std::vector<App::Component::Chemistry::Chain *>();
		std::vector<App::Component::Chemistry::Residue *> residues
			= std::vector<App::Component::Chemistry::Residue *>();
		std::vector<App::Component::Chemistry::Atom *> atoms = std::vector<App::Component::Chemistry::Atom *>();

		std::vector<App::Component::Video::Path *>		   paths = std::vector<App::Component::Video::Path *>();
		std::vector<App::Component::Object3D::Viewpoint *> viewpoints
			= std::vector<App::Component::Object3D::Viewpoint *>();
		std::vector<App::Component::Object3D::Label *> labels = std::vector<App::Component::Object3D::Label *>();

		for ( const App::Core::Model::ID modelId : _models )
		{
			App::VTX_ID modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

			if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
			{
				App::Component::Chemistry::Molecule & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
			{
				App::Component::Chemistry::Category & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
			{
				App::Component::Chemistry::Chain & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
			{
				App::Component::Chemistry::Residue & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_ATOM )
			{
				App::Component::Chemistry::Atom & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( modelId );
				atoms.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_PATH )
			{
				App::Component::Video::Path & path
					= VTX::MVC_MANAGER().getModel<App::Component::Video::Path>( modelId );
				paths.emplace_back( &path );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_VIEWPOINT )
			{
				App::Component::Object3D::Viewpoint & model
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( modelId );
				viewpoints.emplace_back( &model );
			}
			else if ( Util::App::Label::isLabelType( modelTypeId ) )
			{
				App::Component::Object3D::Label & model
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Label>( modelId );
				labels.emplace_back( &model );
			}
		}

		_selection.selectModels( molecules, categories, chains, residues, atoms, _appendToSelection );
		_selection.selectModels( paths, true );
		_selection.selectModels( viewpoints, true );
		_selection.selectModels( labels, true );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectModels::execute()
	{
		std::vector<App::Component::Chemistry::Molecule *> molecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		std::vector<App::Component::Chemistry::Category *> categories
			= std::vector<App::Component::Chemistry::Category *>();
		std::vector<App::Component::Chemistry::Chain *>	  chains = std::vector<App::Component::Chemistry::Chain *>();
		std::vector<App::Component::Chemistry::Residue *> residues
			= std::vector<App::Component::Chemistry::Residue *>();
		std::vector<App::Component::Chemistry::Atom *> atoms = std::vector<App::Component::Chemistry::Atom *>();

		for ( const App::Core::Model::ID modelId : _models )
		{
			App::VTX_ID modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

			if ( modelTypeId == App::ID::Model::MODEL_MOLECULE )
			{
				App::Component::Chemistry::Molecule & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_CATEGORY )
			{
				App::Component::Chemistry::Category & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_CHAIN )
			{
				App::Component::Chemistry::Chain & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_RESIDUE )
			{
				App::Component::Chemistry::Residue & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == App::ID::Model::MODEL_ATOM )
			{
				App::Component::Chemistry::Atom & model
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Atom>( modelId );
				atoms.emplace_back( &model );
			}
		}

		_selection.unselectModels( molecules, categories, chains, residues, atoms );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectMolecule::execute()
	{
		_selection.selectMolecules( _molecules, _appendToSelection );
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectCategory::execute()
	{
		_selection.selectCategories( _categories, _appendToSelection );
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectChain::execute()
	{
		_selection.selectChains( _chains, _appendToSelection );
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectResidue::execute()
	{
		_selection.selectResidues( _residues, _appendToSelection );
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectAtom::execute()
	{
		_selection.selectAtoms( _atoms, _appendToSelection );
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectMolecule::execute()
	{
		if ( _check )
			_selection.unselectMolecules( _molecules );
		else
			_selection.unselectMoleculesWithCheck( _molecules );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectCategory::execute()
	{
		if ( _check )
			_selection.unselectCategories( _categories );
		else
			_selection.unselectCategoriesWithCheck( _categories );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectChain::execute()
	{
		if ( _check )
			_selection.unselectChains( _chains );
		else
			_selection.unselectChainsWithCheck( _chains );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectResidue::execute()
	{
		if ( _check )
			_selection.unselectResidues( _residues );
		else
			_selection.unselectResiduesWithCheck( _residues );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectAtom::execute()
	{
		if ( _check )
			_selection.unselectAtoms( _atoms );
		else
			_selection.unselectAtomsWithCheck( _atoms );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void ClearSelection::execute()
	{
		_selection.clear();
		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}
	void SelectViewpoint::execute() { _selection.selectModels( _viewpoints, _appendToSelection ); }

	void UnselectViewpoint::execute()
	{
		if ( _check )
			_selection.unselectModels( _viewpoints );
		else
			_selection.unselectModelsWithCheck( _viewpoints );

		// App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
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

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}
	void ChangeVisibility::show( const bool p_show )
	{
		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );

			if ( _selection.isMoleculeFullySelected( molecule ) )
			{
				Util::App::Molecule::show( molecule, p_show, false, true );
			}
			else
			{
				molecule.setVisible( true );

				for ( const App::Application::Selection::SelectionModel::PairChainIds & chainIds : molIds.second )
				{
					App::Component::Chemistry::Chain & chain = *molecule.getChain( chainIds.first );
					if ( _selection.isChainFullySelected( chain ) )
					{
						Util::App::Molecule::show( chain, p_show, false, false, false );
					}
					else
					{
						chain.setVisible( true );
						for ( const App::Application::Selection::SelectionModel::PairResidueIds & residueIds :
							  chainIds.second )
						{
							App::Component::Chemistry::Residue & residue = *molecule.getResidue( residueIds.first );
							if ( _selection.isResidueFullySelected( residue ) )
							{
								Util::App::Molecule::show( residue, p_show, false, false, false );
							}
							else
							{
								residue.setVisible( true );
								for ( const uint atomId : residueIds.second )
								{
									App::Component::Chemistry::Atom * const atom = molecule.getAtom( atomId );
									atom->setVisible( p_show, false );
								}
							}
						}
					}
				}
			}

			for ( App::Component::Chemistry::Category * const category : molecule.getCategories() )
				category->updateVisibilityState();

			molecule.notifyVisibilityChange();
			molecule.refreshVisibilities();
			molecule.computeRepresentationTargets();
		}
	}

	void ChangeVisibility::solo()
	{
		const App::Application::Scene::MapMoleculePtrFloat & moleculesInScene = App::VTXApp::get().getScene().getMolecules();
		const App::Application::Selection::SelectionModel::MapMoleculeIds & moleculesInSelection
			= _selection.getMoleculesMap();

		for ( const App::Application::Scene::PairMoleculePtrFloat & sceneMolecule : moleculesInScene )
		{
			App::Component::Chemistry::Molecule * const									molecule = sceneMolecule.first;
			App::Application::Selection::SelectionModel::MapMoleculeIds::const_iterator itSelection
				= moleculesInSelection.find( molecule->getId() );

			if ( itSelection != moleculesInSelection.end() )
			{
				molecule->setVisible( true, false );

				App::Application::Selection::SelectionModel::MapChainIds::const_iterator itChainSelection
					= itSelection->second.cbegin();

				for ( uint iChain = 0; iChain < molecule->getChainCount(); iChain++ )
				{
					App::Component::Chemistry::Chain * const chain = molecule->getChain( iChain );

					if ( chain == nullptr )
						continue;

					if ( itChainSelection != itSelection->second.cend() && iChain == itChainSelection->first )
					{
						chain->setVisible( true, false );

						App::Application::Selection::SelectionModel::MapResidueIds::const_iterator itResidueSelection
							= itChainSelection->second.cbegin();

						for ( uint iResidue = chain->getIndexFirstResidue(); iResidue <= chain->getIndexLastResidue();
							  iResidue++ )
						{
							App::Component::Chemistry::Residue * const residue = molecule->getResidue( iResidue );

							if ( residue == nullptr )
								continue;

							if ( itResidueSelection != itChainSelection->second.cend()
								 && iResidue == itResidueSelection->first )
							{
								residue->setVisible( true, false );

								App::Application::Selection::SelectionModel::VecAtomIds::const_iterator itAtomSelection
									= itResidueSelection->second.cbegin();

								for ( uint iAtom = residue->getIndexFirstAtom();
									  iAtom < residue->getIndexFirstAtom() + residue->getAtomCount();
									  iAtom++ )
								{
									App::Component::Chemistry::Atom * const atom = molecule->getAtom( iAtom );

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

			for ( App::Component::Chemistry::Category * const category : molecule->getCategories() )
				category->updateVisibilityState();

			molecule->notifyVisibilityChange();
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

	void ToggleWatersVisibility::execute()
	{
		bool showWater = true;

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			showWater = showWater && !molecule.showWater();
		}

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowWater( showWater );
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleSolventVisibility::execute()
	{
		bool showSolvent = true;

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			showSolvent = showSolvent && !molecule.showSolvent();
		}

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowSolvent( showSolvent );
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleHydrogensVisibility::execute()
	{
		bool showHydrogen = true;

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			showHydrogen = showHydrogen && !molecule.showHydrogen();
		}

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowHydrogen( showHydrogen );
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleIonsVisibility::execute()
	{
		bool showIons = true;

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			showIons = showIons && !molecule.showIon();
		}

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowIon( showIons );
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleTrajectoryPlaying::execute()
	{
		bool play = true;

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );
			if ( molecule.hasTrajectory() )
				play = play && !molecule.isPlaying();
		}

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );

			if ( molecule.hasTrajectory() )
			{
				if ( molecule.isAtEndOfTrajectoryPlay() && play )
					molecule.resetTrajectoryPlay();

				molecule.setIsPlaying( play );
			}
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		App::Application::Representation::RepresentationPreset * const preset
			= App::Application::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		App::Application::Representation::RepresentationManager::get().instantiateRepresentations( preset, _selection );

		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		for ( const App::Core::Model::ID & selectedObjectID : _selection.getItems() )
		{
			const App::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( selectedObjectID );

			if ( modelTypeID == App::ID::Model::MODEL_MOLECULE )
			{
				const App::Component::Chemistry::Molecule & source
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( selectedObjectID );
				const int nbFrames = source.getFrameCount();

				if ( _frame == App::Component::Chemistry::GeneratedMolecule::ALL_FRAMES_SEPARATED_INDEX )
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
	void Copy::_copyFrame( const App::Component::Chemistry::Molecule &		   p_source,
						   const App::Application::Selection::SelectionModel & p_selection,
						   const int										   p_frame )
	{
		App::Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromSelection( _selection, p_source.getId(), p_frame );

		Util::Chrono chrono;

		chrono.start();

		generatedMolecule->applyTransform( p_source.getTransform() );
		App::VTXApp::get().getScene().addMolecule( generatedMolecule );

		chrono.stop();
		VTX_DEBUG( "Molecule " + generatedMolecule->getDisplayName() + " copied in " + chrono.elapsedTimeStr() );
	}

	void Extract::execute()
	{
		App::Application::Selection::SelectionModel * const tmpSelection
			= VTX::MVC_MANAGER().instantiateModel<App::Application::Selection::SelectionModel>();
		_selection.moveDataTo( *tmpSelection );
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().clear();

		std::vector<App::Component::Chemistry::Molecule *> generatedMolecules
			= std::vector<App::Component::Chemistry::Molecule *>();
		generatedMolecules.reserve( tmpSelection->getMoleculesMap().size() );

		for ( const App::Application::Selection::SelectionModel::PairMoleculeIds & moleculeSelectionData :
			  tmpSelection->getMoleculesMap() )
		{
			const App::Core::Model::ID &		  idMolSource = moleculeSelectionData.first;
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( idMolSource );

			if ( tmpSelection->isMoleculeFullySelected( molecule ) )
				continue;

			App::Component::Chemistry::GeneratedMolecule * const generatedMolecule
				= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

			generatedMolecule->extractFromSelection( *tmpSelection, idMolSource );
			App::VTXApp::get().getScene().addMolecule( generatedMolecule );
			generatedMolecules.emplace_back( generatedMolecule );
		}

		VTX::MVC_MANAGER().deleteModel( tmpSelection );

		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecules(
			generatedMolecules );

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		std::vector<App::Component::Chemistry::Molecule *> moleculesToDelete
			= std::vector<App::Component::Chemistry::Molecule *>();

		const std::set<App::Core::Model::ID>						itemsToDeleteCopy = _selection.getItems();
		App::Application::Selection::SelectionModel::MapMoleculeIds moleculeMapToDeleteCopy
			= _selection.getMoleculesMap();
		_selection.clear();

		for ( const App::Core::Model::ID & selectedObjectID : itemsToDeleteCopy )
		{
			const App::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( selectedObjectID );

			if ( modelTypeID == App::ID::Model::MODEL_MOLECULE )
			{
				const App::Application::Selection::SelectionModel::PairMoleculeIds & molIds
					= *moleculeMapToDeleteCopy.find( selectedObjectID );

				App::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( molIds.first );

				if ( molIds.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					moleculesToDelete.emplace_back( &molecule );
					continue;
				}

				for ( const App::Application::Selection::SelectionModel::PairChainIds & chainIds : molIds.second )
				{
					App::Component::Chemistry::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						molecule.removeChain( chain.getIndex(), true, true, false );
						continue;
					}

					for ( const App::Application::Selection::SelectionModel::PairResidueIds & residueIds :
						  chainIds.second )
					{
						App::Component::Chemistry::Residue & residue = *molecule.getResidue( residueIds.first );

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
			else if ( modelTypeID == App::ID::Model::MODEL_PATH )
			{
				App::Component::Video::Path & path
					= VTX::MVC_MANAGER().getModel<App::Component::Video::Path>( selectedObjectID );

				VTX::App::VTXApp::get().getScene().removePath( &path );
				VTX::MVC_MANAGER().deleteModel( &path );
			}
			else if ( modelTypeID == App::ID::Model::MODEL_VIEWPOINT )
			{
				App::Component::Object3D::Viewpoint & viewpoint
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( selectedObjectID );

				App::Component::Video::Path * const path = viewpoint.getPathPtr();
				path->removeViewpoint( &viewpoint );
				VTX::MVC_MANAGER().deleteModel( &viewpoint );

				path->refreshAllDurations();
			}
			else if ( Util::App::Label::isLabelType( modelTypeID ) )
			{
				App::Component::Object3D::Label & label
					= VTX::MVC_MANAGER().getModel<App::Component::Object3D::Label>( selectedObjectID );
				App::VTXApp::get().getScene().removeLabel( &label );
				VTX::MVC_MANAGER().deleteModel<App::Component::Object3D::Label>( &label );
			}
		}

		for ( App::Component::Chemistry::Molecule * const moleculeToDelete : moleculesToDelete )
		{
			App::VTXApp::get().getScene().removeMolecule( moleculeToDelete );
			VTX::MVC_MANAGER().deleteModel( moleculeToDelete );
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Action::Selection
