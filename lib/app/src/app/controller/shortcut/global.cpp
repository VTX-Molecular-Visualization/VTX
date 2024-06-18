#include "app/controller/shortcut/global.hpp"
#include "app/core/input/keys.hpp"
#include <app/action/application.hpp>

namespace VTX::App::Controller::Shortcut
{
	void Global::init()
	{
		BaseShortcutController::init();

		referenceShortcut( { { Core::Input::Key::Key_N, ( Core::Input::ModifierEnum::Ctrl ) },
							 Core::Input::Shortcut::callAction<App::Action::Application::NewScene>() } );

		referenceShortcut( { { Core::Input::Key::Key_S, Core::Input::ModifierEnum::Ctrl },
							 Core::Input::Shortcut::callAction<App::Action::Application::SaveScene>() } );

		referenceShortcut( { { Core::Input::Key::Key_S, ( Core::Input::ModifierEnum::Ctrl_Shift ) },
							 Core::Input::Shortcut::callAction<App::Action::Application::SaveScene>() } );
	}

} // namespace VTX::App::Controller::Shortcut
