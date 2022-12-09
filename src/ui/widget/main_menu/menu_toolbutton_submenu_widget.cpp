#include "menu_toolbutton_submenu_widget.hpp"

namespace VTX::UI::Widget::MainMenu
{
	void MenuToolButtonSubmenuWidget::_setupUi( const QString & p_name )
	{
		MenuToolButtonWidget::_setupUi( p_name );

		_submenu = new QMenu( this );
		setPopupMode( QToolButton::ToolButtonPopupMode::MenuButtonPopup );
		setMenu( _submenu );
	}
	void MenuToolButtonSubmenuWidget::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		setTriggerAction( this, &MenuToolButtonSubmenuWidget::_triggerDefaultAction );
	}

	void MenuToolButtonSubmenuWidget::localize() {}

	void MenuToolButtonSubmenuWidget::addAction( QAction * const p_action ) { _submenu->addAction( p_action ); }

	void MenuToolButtonSubmenuWidget::setDefaultAction( QAction * const p_defaultAction )
	{
		_setDefaultAction( p_defaultAction );
	}
	void MenuToolButtonSubmenuWidget::setSubmenu( QMenu * const p_submenu )
	{
		setMenu( p_submenu );
		delete _submenu;
		_submenu = p_submenu;
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
