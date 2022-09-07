#include "menu_toolbutton_submenu_widget.hpp"

namespace VTX::UI::Widget::MainMenu
{
	void MenuToolButtonSubmenuWidget::addAction( QAction * const p_action ) { _submenu->addAction( p_action ); }
	void MenuToolButtonSubmenuWidget::_setupUi( const QString & p_name )
	{
		_submenu = new QMenu( this );
		setPopupMode( QToolButton::ToolButtonPopupMode::DelayedPopup );
		setMenu( _submenu );
	}
	void MenuToolButtonSubmenuWidget::_setupSlots()
	{
		setTriggerAction( this, &MenuToolButtonSubmenuWidget::_triggerDefaultAction );
	}

	void MenuToolButtonSubmenuWidget::_setDefaultAction( QAction * const p_action )
	{
		_defaultAction = p_action;
		setText( _defaultAction->text() );
	}

	void MenuToolButtonSubmenuWidget::_triggerDefaultAction() const
	{
		if ( _defaultAction != nullptr )
			_defaultAction->trigger();
	}

	void MenuToolButtonSubmenuWidget::_actionHasBeenTriggered( UIAction::SelfReferencedAction * const p_action )
	{
		_setDefaultAction( p_action );
	}

} // namespace VTX::UI::Widget::MainMenu