#include "ui/qt/controller/visualization_shortcut.hpp"
#include "ui/action/visualization.hpp"

namespace VTX::UI::QT::Controller
{
	void VisualizationShortcut::init()
	{
		BaseShortcutController::init();

		referenceShortcut( { { Core::Input::Key::Key_F1 },
							 Core::Input::Shortcut::callAction<Action::Visualization::ToggleCameraController>() } );
	}

} // namespace VTX::UI::QT::Controller
