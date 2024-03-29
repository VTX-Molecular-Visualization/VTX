#include "ui/action/visualization.hpp"
#include "ui/action/animation.hpp"
#include "ui/core/animation/animation_system.hpp"
#include "ui/internal/controller/shortcut/visualization.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/system/action_manager.hpp>

namespace VTX::UI::Internal::Controller::Shortcut
{
	void Visualization::init()
	{
		BaseShortcutController::init();

		referenceShortcut( { { Core::Input::Key::Key_F1 },
							 Core::Input::Shortcut::callAction<Action::Visualization::ToggleCameraController>() } );

		referenceShortcut( { { Core::Input::Key::Key_F1, Core::Input::ModifierEnum::Ctrl },
							 Core::Input::Shortcut::callAction<Action::Animation::ResetCamera>() } );

		referenceShortcut( { { Core::Input::Key::Key_O },
							 [ this ]()
							 {
								 if ( App::CURRENT_SELECTION().isEmpty() )
									 _callOrientOnScene();
								 else
									 _callOrientOnSelection();
							 } } );

		// TODO Maybe move that in a controller dedicated for the animator which activate / deactivate itself when the
		// Animator Play / Stop
		referenceShortcut( { { Core::Input::Key::Key_Escape },
							 [ this ]()
							 {
								 if ( ANIMATION_SYSTEM().isPlaying() )
								 {
									 ANIMATION_SYSTEM().stop();
								 }
							 } } );
	}

	void Visualization::_callOrientOnScene() const
	{
		App::VTX_ACTION().execute<Action::Animation::Orient>( App::SCENE().getAABB() );
	}
	void Visualization::_callOrientOnSelection() const
	{
		App::VTX_ACTION().execute<Action::Animation::Orient>( App::CURRENT_SELECTION().getAABB() );
	}

} // namespace VTX::UI::Internal::Controller::Shortcut
