#include "ui/qt/controller/debug_shortcut.hpp"
#include "ui/action/visualization.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <app/action/application.hpp>
#include <app/action/debug.hpp>
#include <app/action/scene.hpp>

namespace VTX::UI::QT::Controller
{
	void DebugShortcut::init()
	{
		referenceShortcut( { { Input::Key::Key_F9 },
							 Shortcut::callAction<App::Action::Debug::ToggleRendererActivation>() } );
		referenceShortcut( { { Input::Key::Key_F10 }, Shortcut::callAction<App::Action::Debug::RefreshSES>() } );
		referenceShortcut( { { Input::Key::Key_F7 }, Shortcut::callAction<App::Action::Debug::ChangeRenderMode>() } );
		referenceShortcut( { { Input::Key::Key_Space }, Shortcut::callAction<App::Action::Debug::PrintCameraInfo>() } );

		INPUT_MANAGER().onKeyPressed.addCallback( this, [ this ]( Input::Key p_key ) { _onKeyPressed( p_key ); } );
	}

	void DebugShortcut::referenceShortcut( const Shortcut & p_shortcut )
	{
		_mapShortcuts.referenceShortcut( p_shortcut );
	}

	void DebugShortcut::_onKeyPressed( Input::Key p_key ) const
	{
		if ( !isActive() || !isTargetWidgetFocused() )
			return;

		const Shortcut * const shortcutPtr = _mapShortcuts.tryGet( p_key, INPUT_MANAGER().getCurrentModifiers() );

		if ( shortcutPtr )
			shortcutPtr->trigger();
	}

} // namespace VTX::UI::QT::Controller
