#include "app/controller/shortcut/debug.hpp"
#include "app/action/debug.hpp"
#include "app/core/input/keys.hpp"

namespace VTX::App::Controller::Shortcut
{
	void Debug::init()
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

} // namespace VTX::App::Controller::Shortcut
