#include "app/old/action/molecule.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/component/chemistry/generated_molecule.hpp"
#include "app/old/mvc.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/util/molecule.hpp"
#include "app/old/vtx_app.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Action::Molecule
{
	void RefreshSolventExcludedSurface::execute()
	{
		_molecule.refreshSolventExcludedSurfaces();
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

	void ChangeColor::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			molecule->setColor( _color );
			molecule->refreshColors();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		for ( Component::Generic::BaseVisible * const visible : _visibles )
		{
			Component::Chemistry::Molecule * molecule = static_cast<Component::Chemistry::Molecule *>( visible );

			switch ( _mode )
			{
			case VISIBILITY_MODE::SHOW:
			case VISIBILITY_MODE::HIDE:
			case VISIBILITY_MODE::ALL:
				Util::App::Old::Molecule::show( *molecule, _getVisibilityBool( *molecule ), true, true );
				break;
			case VISIBILITY_MODE::SOLO: Util::App::Old::Molecule::solo( *molecule, false ); break;
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		Application::Representation::RepresentationPreset * const preset
			= Application::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		App::Old::Application::Representation::RepresentationManager::get().instantiateRepresentations( preset, _molecules );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			App::Old::Application::Representation::RepresentationManager::get().instantiateDefaultRepresentation(
				*molecule );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveChildrenRepresentations::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			molecule->removeChildrenRepresentations();
			molecule->computeAllRepresentationData();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeFPS::execute()
	{
		for ( Component::Chemistry::Molecule * molecule : _molecules )
			molecule->setFPS(
				Util::Math::clamp( _fps, VTX::App::Old::Application::Setting::MIN_TRAJECTORY_SPEED, VTX::App::Old::Application::Setting::MAX_TRAJECTORY_SPEED ) );
	}

	void ChangeFrame::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			molecule->setFrame( Util::Math::clamp( _frame, 0, (int)molecule->getFrameCount() - 1 ) );
			if ( _pause )
				molecule->setIsPlaying( false );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void PreviousFrame::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			const int previousFrame		   = molecule->getFrame() - 1;
			const int clampedPreviousFrame = previousFrame < 0 ? 0 : previousFrame;

			molecule->setFrame( clampedPreviousFrame );

			if ( _pause )
				molecule->setIsPlaying( false );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void NextFrame::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			const int nextFrame		   = molecule->getFrame() + 1;
			const int lastFrame		   = molecule->getFrameCount() - 1;
			const int clampedNextFrame = nextFrame > lastFrame ? lastFrame : nextFrame;

			molecule->setFrame( clampedNextFrame );

			if ( _pause )
				molecule->setIsPlaying( false );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeIsPlaying::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
		{
			if ( molecule->isAtEndOfTrajectoryPlay() && _isPlaying )
				molecule->resetTrajectoryPlay();

			molecule->setIsPlaying( _isPlaying );
		}
	}

	void ChangePlayMode::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			molecule->setPlayMode( _playMode );
	}

	void ChangeShowIon::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			molecule->setShowIon( _showIon );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowSolvent::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			molecule->setShowSolvent( _showSolvent );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowWater::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			molecule->setShowWater( _showWater );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeShowHydrogen::execute()
	{
		for ( Component::Chemistry::Molecule * const molecule : _molecules )
			molecule->setShowHydrogen( _showHydrogen );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		App::Old::VTXApp::get().getScene().removeMolecule( &_molecule );
		VTX::MVC_MANAGER().deleteModel( &_molecule );
	}

	void Copy::execute()
	{
		Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromMolecule( _target );
		generatedMolecule->applyTransform( _target.getTransform() );

		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Rename::execute() { _target.setDisplayName( _newName ); }
	void Rename::undo() { _target.setDisplayName( _oldName ); }

	void ApplyRepresentation::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().applyRepresentation(
			_molecules, _representation, _flag );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Old::Action::Molecule
