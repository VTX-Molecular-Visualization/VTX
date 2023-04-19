#include "app/action/molecule.hpp"
#include "app/old_app/model/generated_molecule.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/representation/representation_library.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/setting.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/math.hpp>

namespace VTX::Action::Molecule
{
	void RefreshSolventExcludedSurface::execute()
	{
		_molecule.refreshSolventExcludedSurfaces();
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeColor::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			molecule->setColor( _color );
			molecule->refreshColors();
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		for ( Generic::BaseVisible * const visible : _visibles )
		{
			Model::Molecule * molecule = static_cast<Model::Molecule *>( visible );

			switch ( _mode )
			{
			case VISIBILITY_MODE::SHOW:
			case VISIBILITY_MODE::HIDE:
			case VISIBILITY_MODE::ALL:
				Util::Molecule::show( *molecule, _getVisibilityBool( *molecule ), true, true );
				break;
			case VISIBILITY_MODE::SOLO: Util::Molecule::solo( *molecule, false ); break;
			}
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		Model::Representation::Representation * const preset
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		Representation::RepresentationManager::get().instantiateRepresentations( preset, _molecules );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			VTX::Representation::RepresentationManager::get().instantiateDefaultRepresentation( *molecule );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveChildrenRepresentations::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			molecule->removeChildrenRepresentations();
			molecule->computeAllRepresentationData();
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeFPS::execute()
	{
		for ( Model::Molecule * molecule : _molecules )
			molecule->setFPS(
				Util::Math::clamp( _fps, VTX::Setting::MIN_TRAJECTORY_SPEED, VTX::Setting::MAX_TRAJECTORY_SPEED ) );
	}

	void ChangeFrame::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			molecule->setFrame( Util::Math::clamp( _frame, 0, (int)molecule->getFrameCount() - 1 ) );
			if ( _pause )
				molecule->setIsPlaying( false );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void PreviousFrame::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			const int previousFrame		   = molecule->getFrame() - 1;
			const int clampedPreviousFrame = previousFrame < 0 ? 0 : previousFrame;

			molecule->setFrame( clampedPreviousFrame );

			if ( _pause )
				molecule->setIsPlaying( false );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void NextFrame::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			const int nextFrame		   = molecule->getFrame() + 1;
			const int lastFrame		   = molecule->getFrameCount() - 1;
			const int clampedNextFrame = nextFrame > lastFrame ? lastFrame : nextFrame;

			molecule->setFrame( clampedNextFrame );

			if ( _pause )
				molecule->setIsPlaying( false );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeIsPlaying::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
		{
			if ( molecule->isAtEndOfTrajectoryPlay() && _isPlaying )
				molecule->resetTrajectoryPlay();

			molecule->setIsPlaying( _isPlaying );
		}
	}

	void ChangePlayMode::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			molecule->setPlayMode( _playMode );
	}

	void ChangeShowIon::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			molecule->setShowIon( _showIon );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowSolvent::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			molecule->setShowSolvent( _showSolvent );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowWater::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			molecule->setShowWater( _showWater );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowHydrogen::execute()
	{
		for ( Model::Molecule * const molecule : _molecules )
			molecule->setShowHydrogen( _showHydrogen );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTXApp::get().getScene().removeMolecule( &_molecule );
		MVC::MvcManager::get().deleteModel( &_molecule );
	}

	void Copy::execute()
	{
		Model::GeneratedMolecule * generatedMolecule
			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->copyFromMolecule( _target );
		generatedMolecule->applyTransform( _target.getTransform() );

		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Rename::execute() { _target.setDisplayName( _newName ); }
	void Rename::undo() { _target.setDisplayName( _oldName ); }

	void ApplyRepresentation::execute()
	{
		Representation::RepresentationManager::get().applyRepresentation( _molecules, _representation, _flag );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::Action::Molecule
