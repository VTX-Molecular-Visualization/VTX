#include "ui/qt/controller/base_shortcut_controller.hpp"
#include "ui/qt/input/input_manager.hpp"

namespace VTX::UI::QT::Controller
{
	void BaseShortcutController::init()
	{
		INPUT_MANAGER().onKeyPressed += [ this ]( Core::Input::Key p_key ) { _onKeyPressed( p_key ); };
	}

	void BaseShortcutController::referenceShortcut( const Core::Input::Shortcut & p_shortcut )
	{
		_mapShortcuts.referenceShortcut( p_shortcut );
	}

	void BaseShortcutController::_onKeyPressed( Input::Key p_key ) const
	{
		if ( !isActive() || !isTargetWidgetFocused() )
			return;

		const Core::Input::Shortcut * const shortcutPtr
			= _mapShortcuts.tryGet( p_key, INPUT_MANAGER().getCurrentModifiers() );

		if ( shortcutPtr )
			shortcutPtr->trigger();
	}

} // namespace VTX::UI::QT::Controller
