#include "ui/qt/controller/global_shortcut.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <app/action/application.hpp>

namespace VTX::UI::QT::Controller
{
	void GlobalShortcut::init()
	{
		referenceShortcut( { { Input::Key::Key_N, ( Input::ModifierEnum::Ctrl ) },
							 Shortcut::callAction<App::Action::Application::NewScene>() } );

		referenceShortcut( { { Input::Key::Key_S, Input::ModifierEnum::Ctrl },
							 Shortcut::callAction<App::Action::Application::SaveScene>() } );

		referenceShortcut( { { Input::Key::Key_S, ( Input::ModifierEnum::Ctrl_Shift ) },
							 Shortcut::callAction<App::Action::Application::SaveScene>() } );

		INPUT_MANAGER().onKeyPressed.addCallback( this, [ this ]( Input::Key p_key ) { _onKeyPressed( p_key ); } );
	}

	void GlobalShortcut::referenceShortcut( const Shortcut & p_shortcut )
	{
		_mapShortcuts.referenceShortcut( p_shortcut );
	}

	void GlobalShortcut::_onKeyPressed( Input::Key p_key ) const
	{
		if ( !isActive() || !isTargetWidgetFocused() )
			return;

		const Shortcut * const shortcutPtr = _mapShortcuts.tryGet( p_key, INPUT_MANAGER().getCurrentModifiers() );

		if ( shortcutPtr )
			shortcutPtr->trigger();
	}

} // namespace VTX::UI::QT::Controller
