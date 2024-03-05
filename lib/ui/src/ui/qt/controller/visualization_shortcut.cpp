#include "ui/qt/controller/visualization_shortcut.hpp"
#include "ui/action/animation.hpp"
#include "ui/action/visualization.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/system/action_manager.hpp>

namespace VTX::UI::QT::Controller
{
	void VisualizationShortcut::init()
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
	}

	void VisualizationShortcut::_callOrientOnScene() const
	{
		App::VTX_ACTION().execute<Action::Animation::Orient>( App::SCENE().getAABB() );
	}
	void VisualizationShortcut::_callOrientOnSelection() const
	{
		App::VTX_ACTION().execute<Action::Animation::Orient>( App::CURRENT_SELECTION().getAABB() );
	}

} // namespace VTX::UI::QT::Controller
