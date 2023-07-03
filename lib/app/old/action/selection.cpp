#include "app/old/action/selection.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/component/object3d/label.hpp"
#include "app/old/component/video/path.hpp"
#include "app/old/mvc.hpp"
#include "app/old/util/label.hpp"
#include "app/old/util/molecule.hpp"
#include "app/old/vtx_app.hpp"
#include <util/chrono.hpp>

namespace VTX::App::Old::Action::Selection
{
	void SelectAll::execute()
	{
		const App::Old::Application::Scene::MapMoleculePtrFloat & sceneMolecules
			= App::Old::VTXApp::get().getScene().getMolecules();
		std::vector<App::Old::Component::Chemistry::Molecule *> molecules
			= std::vector<App::Old::Component::Chemistry::Molecule *>();
		molecules.reserve( sceneMolecules.size() );

		for ( const std::pair<App::Old::Component::Chemistry::Molecule * const, float> & moleculePair : sceneMolecules )
		{
			molecules.emplace_back( moleculePair.first );
		}

		const App::Old::Application::Scene::VectorPathPtr & paths		  = App::Old::VTXApp::get().getScene().getPaths();
		size_t										   viewpointCount = 0;
		for ( const App::Old::Component::Video::Path * const path : paths )
			viewpointCount += path->getViewpoints().size();

		std::vector<App::Old::Component::Object3D::Viewpoint *> viewpoints
			= std::vector<App::Old::Component::Object3D::Viewpoint *>();
		viewpoints.reserve( viewpointCount );
		for ( App::Old::Component::Video::Path * const path : paths )
		{
			for ( App::Old::Component::Object3D::Viewpoint * const viewpoint : path->getViewpoints() )
			{
				viewpoints.emplace_back( viewpoint );
			}
		}

		const App::Old::Application::Scene::VectorMeshTrianglePtr & meshes = App::Old::VTXApp::get().getScene().getMeshes();
		const App::Old::Application::Scene::VectorLabelPtr &		   labels = App::Old::VTXApp::get().getScene().getLabels();

		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecules( molecules,
																									   false );
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( viewpoints, true );
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( meshes, true );
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectModels( labels, true );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectModels::execute()
	{
		std::vector<App::Old::Component::Chemistry::Molecule *> molecules
			= std::vector<App::Old::Component::Chemistry::Molecule *>();
		std::vector<App::Old::Component::Chemistry::Category *> categories
			= std::vector<App::Old::Component::Chemistry::Category *>();
		std::vector<App::Old::Component::Chemistry::Chain *>	  chains = std::vector<App::Old::Component::Chemistry::Chain *>();
		std::vector<App::Old::Component::Chemistry::Residue *> residues
			= std::vector<App::Old::Component::Chemistry::Residue *>();
		std::vector<App::Old::Component::Chemistry::Atom *> atoms = std::vector<App::Old::Component::Chemistry::Atom *>();

		std::vector<App::Old::Component::Video::Path *>		   paths = std::vector<App::Old::Component::Video::Path *>();
		std::vector<App::Old::Component::Object3D::Viewpoint *> viewpoints
			= std::vector<App::Old::Component::Object3D::Viewpoint *>();
		std::vector<App::Old::Component::Object3D::Label *> labels = std::vector<App::Old::Component::Object3D::Label *>();

		for ( const App::Old::Core::Model::ID modelId : _models )
		{
			App::Old::VTX_ID modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

			if ( modelTypeId == App::Old::ID::Model::MODEL_MOLECULE )
			{
				App::Old::Component::Chemistry::Molecule & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_CATEGORY )
			{
				App::Old::Component::Chemistry::Category & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_CHAIN )
			{
				App::Old::Component::Chemistry::Chain & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_RESIDUE )
			{
				App::Old::Component::Chemistry::Residue & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_ATOM )
			{
				App::Old::Component::Chemistry::Atom & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( modelId );
				atoms.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_PATH )
			{
				App::Old::Component::Video::Path & path
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Video::Path>( modelId );
				paths.emplace_back( &path );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_VIEWPOINT )
			{
				App::Old::Component::Object3D::Viewpoint & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Viewpoint>( modelId );
				viewpoints.emplace_back( &model );
			}
			else if ( Util::App::Old::Label::isLabelType( modelTypeId ) )
			{
				App::Old::Component::Object3D::Label & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Label>( modelId );
				labels.emplace_back( &model );
			}
		}

		_selection.selectModels( molecules, categories, chains, residues, atoms, _appendToSelection );
		_selection.selectModels( paths, true );
		_selection.selectModels( viewpoints, true );
		_selection.selectModels( labels, true );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectModels::execute()
	{
		std::vector<App::Old::Component::Chemistry::Molecule *> molecules
			= std::vector<App::Old::Component::Chemistry::Molecule *>();
		std::vector<App::Old::Component::Chemistry::Category *> categories
			= std::vector<App::Old::Component::Chemistry::Category *>();
		std::vector<App::Old::Component::Chemistry::Chain *>	  chains = std::vector<App::Old::Component::Chemistry::Chain *>();
		std::vector<App::Old::Component::Chemistry::Residue *> residues
			= std::vector<App::Old::Component::Chemistry::Residue *>();
		std::vector<App::Old::Component::Chemistry::Atom *> atoms = std::vector<App::Old::Component::Chemistry::Atom *>();

		for ( const App::Old::Core::Model::ID modelId : _models )
		{
			App::Old::VTX_ID modelTypeId = VTX::MVC_MANAGER().getModelTypeID( modelId );

			if ( modelTypeId == App::Old::ID::Model::MODEL_MOLECULE )
			{
				App::Old::Component::Chemistry::Molecule & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( modelId );
				molecules.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_CATEGORY )
			{
				App::Old::Component::Chemistry::Category & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Category>( modelId );
				categories.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_CHAIN )
			{
				App::Old::Component::Chemistry::Chain & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Chain>( modelId );
				chains.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_RESIDUE )
			{
				App::Old::Component::Chemistry::Residue & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Residue>( modelId );
				residues.emplace_back( &model );
			}
			else if ( modelTypeId == App::Old::ID::Model::MODEL_ATOM )
			{
				App::Old::Component::Chemistry::Atom & model
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( modelId );
				atoms.emplace_back( &model );
			}
		}

		_selection.unselectModels( molecules, categories, chains, residues, atoms );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectMolecule::execute()
	{
		_selection.selectMolecules( _molecules, _appendToSelection );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectCategory::execute()
	{
		_selection.selectCategories( _categories, _appendToSelection );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectChain::execute()
	{
		_selection.selectChains( _chains, _appendToSelection );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectResidue::execute()
	{
		_selection.selectResidues( _residues, _appendToSelection );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void SelectAtom::execute()
	{
		_selection.selectAtoms( _atoms, _appendToSelection );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectMolecule::execute()
	{
		if ( _check )
			_selection.unselectMolecules( _molecules );
		else
			_selection.unselectMoleculesWithCheck( _molecules );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectCategory::execute()
	{
		if ( _check )
			_selection.unselectCategories( _categories );
		else
			_selection.unselectCategoriesWithCheck( _categories );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectChain::execute()
	{
		if ( _check )
			_selection.unselectChains( _chains );
		else
			_selection.unselectChainsWithCheck( _chains );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectResidue::execute()
	{
		if ( _check )
			_selection.unselectResidues( _residues );
		else
			_selection.unselectResiduesWithCheck( _residues );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void UnselectAtom::execute()
	{
		if ( _check )
			_selection.unselectAtoms( _atoms );
		else
			_selection.unselectAtomsWithCheck( _atoms );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}

	void ClearSelection::execute()
	{
		_selection.clear();
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
	}
	void SelectViewpoint::execute() { _selection.selectModels( _viewpoints, _appendToSelection ); }

	void UnselectViewpoint::execute()
	{
		if ( _check )
			_selection.unselectModels( _viewpoints );
		else
			_selection.unselectModelsWithCheck( _viewpoints );

		// App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
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

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}
	void ChangeVisibility::show( const bool p_show )
	{
		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );

			if ( _selection.isMoleculeFullySelected( molecule ) )
			{
				Util::App::Old::Molecule::show( molecule, p_show, false, true );
			}
			else
			{
				molecule.setVisible( true );

				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainIds : molIds.second )
				{
					App::Old::Component::Chemistry::Chain & chain = *molecule.getChain( chainIds.first );
					if ( _selection.isChainFullySelected( chain ) )
					{
						Util::App::Old::Molecule::show( chain, p_show, false, false, false );
					}
					else
					{
						chain.setVisible( true );
						for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueIds :
							  chainIds.second )
						{
							App::Old::Component::Chemistry::Residue & residue = *molecule.getResidue( residueIds.first );
							if ( _selection.isResidueFullySelected( residue ) )
							{
								Util::App::Old::Molecule::show( residue, p_show, false, false, false );
							}
							else
							{
								residue.setVisible( true );
								for ( const uint atomId : residueIds.second )
								{
									App::Old::Component::Chemistry::Atom * const atom = molecule.getAtom( atomId );
									atom->setVisible( p_show, false );
								}
							}
						}
					}
				}
			}

			for ( App::Old::Component::Chemistry::Category * const category : molecule.getCategories() )
				category->updateVisibilityState();

			molecule.notifyVisibilityChange();
			molecule.refreshVisibilities();
			molecule.computeRepresentationTargets();
		}
	}

	void ChangeVisibility::solo()
	{
		const App::Old::Application::Scene::MapMoleculePtrFloat & moleculesInScene
			= App::Old::VTXApp::get().getScene().getMolecules();
		const App::Old::Application::Selection::SelectionModel::MapMoleculeIds & moleculesInSelection
			= _selection.getMoleculesMap();

		for ( const App::Old::Application::Scene::PairMoleculePtrFloat & sceneMolecule : moleculesInScene )
		{
			App::Old::Component::Chemistry::Molecule * const									molecule = sceneMolecule.first;
			App::Old::Application::Selection::SelectionModel::MapMoleculeIds::const_iterator itSelection
				= moleculesInSelection.find( molecule->getId() );

			if ( itSelection != moleculesInSelection.end() )
			{
				molecule->setVisible( true, false );

				App::Old::Application::Selection::SelectionModel::MapChainIds::const_iterator itChainSelection
					= itSelection->second.cbegin();

				for ( uint iChain = 0; iChain < molecule->getChainCount(); iChain++ )
				{
					App::Old::Component::Chemistry::Chain * const chain = molecule->getChain( iChain );

					if ( chain == nullptr )
						continue;

					if ( itChainSelection != itSelection->second.cend() && iChain == itChainSelection->first )
					{
						chain->setVisible( true, false );

						App::Old::Application::Selection::SelectionModel::MapResidueIds::const_iterator itResidueSelection
							= itChainSelection->second.cbegin();

						for ( uint iResidue = chain->getIndexFirstResidue(); iResidue <= chain->getIndexLastResidue();
							  iResidue++ )
						{
							App::Old::Component::Chemistry::Residue * const residue = molecule->getResidue( iResidue );

							if ( residue == nullptr )
								continue;

							if ( itResidueSelection != itChainSelection->second.cend()
								 && iResidue == itResidueSelection->first )
							{
								residue->setVisible( true, false );

								App::Old::Application::Selection::SelectionModel::VecAtomIds::const_iterator itAtomSelection
									= itResidueSelection->second.cbegin();

								for ( uint iAtom = residue->getIndexFirstAtom();
									  iAtom < residue->getIndexFirstAtom() + residue->getAtomCount();
									  iAtom++ )
								{
									App::Old::Component::Chemistry::Atom * const atom = molecule->getAtom( iAtom );

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

			for ( App::Old::Component::Chemistry::Category * const category : molecule->getCategories() )
				category->updateVisibilityState();

			molecule->notifyVisibilityChange();
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

	void ToggleWatersVisibility::execute()
	{
		bool showWater = true;

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			showWater = showWater && !molecule.showWater();
		}

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowWater( showWater );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleSolventVisibility::execute()
	{
		bool showSolvent = true;

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			showSolvent = showSolvent && !molecule.showSolvent();
		}

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowSolvent( showSolvent );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleHydrogensVisibility::execute()
	{
		bool showHydrogen = true;

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			showHydrogen = showHydrogen && !molecule.showHydrogen();
		}

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowHydrogen( showHydrogen );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleIonsVisibility::execute()
	{
		bool showIons = true;

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			showIons = showIons && !molecule.showIon();
		}

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			molecule.setShowIon( showIons );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ToggleTrajectoryPlaying::execute()
	{
		bool play = true;

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );
			if ( molecule.hasTrajectory() )
				play = play && !molecule.isPlaying();
		}

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds :
			  _selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );

			if ( molecule.hasTrajectory() )
			{
				if ( molecule.isAtEndOfTrajectoryPlay() && play )
					molecule.resetTrajectoryPlay();

				molecule.setIsPlaying( play );
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		App::Old::Application::Representation::RepresentationPreset * const preset
			= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		App::Old::Application::Representation::RepresentationManager::get().instantiateRepresentations( preset, _selection );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		for ( const App::Old::Core::Model::ID & selectedObjectID : _selection.getItems() )
		{
			const App::Old::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( selectedObjectID );

			if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
			{
				const App::Old::Component::Chemistry::Molecule & source
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( selectedObjectID );
				const int nbFrames = source.getFrameCount();

				if ( _frame == App::Old::Component::Chemistry::GeneratedMolecule::ALL_FRAMES_SEPARATED_INDEX )
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
	void Copy::_copyFrame( const App::Old::Component::Chemistry::Molecule &		   p_source,
						   const App::Old::Application::Selection::SelectionModel & p_selection,
						   const int										   p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		App::Old::Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromSelection( _selection, p_source.getId(), p_frame );

		generatedMolecule->applyTransform( p_source.getTransform() );
		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );

		chrono.stop();
		VTX_DEBUG( "Molecule {0} copied in {1}.", generatedMolecule->getDisplayName(), chrono.elapsedTimeStr() );
	}

	void Extract::execute()
	{
		App::Old::Application::Selection::SelectionModel * const tmpSelection
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Application::Selection::SelectionModel>();
		_selection.moveDataTo( *tmpSelection );
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().clear();

		std::vector<App::Old::Component::Chemistry::Molecule *> generatedMolecules
			= std::vector<App::Old::Component::Chemistry::Molecule *>();
		generatedMolecules.reserve( tmpSelection->getMoleculesMap().size() );

		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & moleculeSelectionData :
			  tmpSelection->getMoleculesMap() )
		{
			const App::Old::Core::Model::ID &		  idMolSource = moleculeSelectionData.first;
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( idMolSource );

			if ( tmpSelection->isMoleculeFullySelected( molecule ) )
				continue;

			App::Old::Component::Chemistry::GeneratedMolecule * const generatedMolecule
				= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

			generatedMolecule->extractFromSelection( *tmpSelection, idMolSource );
			App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );
			generatedMolecules.emplace_back( generatedMolecule );
		}

		VTX::MVC_MANAGER().deleteModel( tmpSelection );

		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecules(
			generatedMolecules );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		std::vector<App::Old::Component::Chemistry::Molecule *> moleculesToDelete
			= std::vector<App::Old::Component::Chemistry::Molecule *>();

		const std::set<App::Old::Core::Model::ID>						itemsToDeleteCopy = _selection.getItems();
		App::Old::Application::Selection::SelectionModel::MapMoleculeIds moleculeMapToDeleteCopy
			= _selection.getMoleculesMap();
		_selection.clear();

		for ( const App::Old::Core::Model::ID & selectedObjectID : itemsToDeleteCopy )
		{
			const App::Old::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( selectedObjectID );

			if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
			{
				const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molIds
					= *moleculeMapToDeleteCopy.find( selectedObjectID );

				App::Old::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molIds.first );

				if ( molIds.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					moleculesToDelete.emplace_back( &molecule );
					continue;
				}

				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainIds : molIds.second )
				{
					App::Old::Component::Chemistry::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						molecule.removeChain( chain.getIndex(), true, true, false );
						continue;
					}

					for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueIds :
						  chainIds.second )
					{
						App::Old::Component::Chemistry::Residue & residue = *molecule.getResidue( residueIds.first );

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
			else if ( modelTypeID == App::Old::ID::Model::MODEL_PATH )
			{
				App::Old::Component::Video::Path & path
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Video::Path>( selectedObjectID );

				VTX::App::Old::VTXApp::get().getScene().removePath( &path );
				VTX::MVC_MANAGER().deleteModel( &path );
			}
			else if ( modelTypeID == App::Old::ID::Model::MODEL_VIEWPOINT )
			{
				App::Old::Component::Object3D::Viewpoint & viewpoint
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Viewpoint>( selectedObjectID );

				App::Old::Component::Video::Path * const path = viewpoint.getPathPtr();
				path->removeViewpoint( &viewpoint );
				VTX::MVC_MANAGER().deleteModel( &viewpoint );

				path->refreshAllDurations();
			}
			else if ( Util::App::Old::Label::isLabelType( modelTypeID ) )
			{
				App::Old::Component::Object3D::Label & label
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Label>( selectedObjectID );
				App::Old::VTXApp::get().getScene().removeLabel( &label );
				VTX::MVC_MANAGER().deleteModel<App::Old::Component::Object3D::Label>( &label );
			}
		}

		for ( App::Old::Component::Chemistry::Molecule * const moleculeToDelete : moleculesToDelete )
		{
			App::Old::VTXApp::get().getScene().removeMolecule( moleculeToDelete );
			VTX::MVC_MANAGER().deleteModel( moleculeToDelete );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Old::Action::Selection
