#include "ui/qt/controller/debug_shortcut.hpp"
#include "ui/core/input/keys.hpp"
#include <app/action/debug.hpp>

namespace VTX::UI::QT::Controller
{
	void DebugShortcut::init()
	{
		BaseShortcutController::init();

		referenceShortcut( { { Core::Input::Key::Key_F9 },
							 Core::Input::Shortcut::callAction<App::Action::Debug::ToggleRendererActivation>() } );
		referenceShortcut( { { Core::Input::Key::Key_F10 },
							 Core::Input::Shortcut::callAction<App::Action::Debug::RefreshSES>() } );
		referenceShortcut( { { Core::Input::Key::Key_F7 },
							 Core::Input::Shortcut::callAction<App::Action::Debug::ChangeRenderMode>() } );
		referenceShortcut( { { Core::Input::Key::Key_Space },
							 Core::Input::Shortcut::callAction<App::Action::Debug::PrintCameraInfo>() } );
	}

} // namespace VTX::UI::QT::Controller
