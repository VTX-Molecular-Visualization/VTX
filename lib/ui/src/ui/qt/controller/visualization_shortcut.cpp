#include "ui/qt/controller/visualization_shortcut.hpp"
#include "ui/action/visualization.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/input/input_manager.hpp"

namespace VTX::UI::QT::Controller
{
	void VisualizationShortcut::init()
	{
		referenceShortcut( { { Input::Key::Key_F1 },
							 Shortcut::callAction<Action::Visualization::ToggleCameraController>() } );

		INPUT_MANAGER().onKeyPressed.addCallback( this, [ this ]( Input::Key p_key ) { _onKeyPressed( p_key ); } );
	}

	void VisualizationShortcut::referenceShortcut( const Shortcut & p_shortcut )
	{
		_mapShortcuts.referenceShortcut( p_shortcut );
	}

	void VisualizationShortcut::_onKeyPressed( Input::Key p_key ) const
	{
		if ( !isActive() || !isTargetWidgetFocused() )
			return;

		const Shortcut * const shortcutPtr = _mapShortcuts.tryGet( p_key, INPUT_MANAGER().getCurrentModifiers() );

		if ( shortcutPtr )
			shortcutPtr->trigger();
	}

} // namespace VTX::UI::QT::Controller
