#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_SUBMENU_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_SUBMENU_WIDGET__

#include "menu_toolbutton_widget.hpp"
#include "ui/ui_action/self_referenced_action.hpp"
#include <QAction>
#include <QMenu>
#include <type_traits>

namespace VTX::UI::Widget::MainMenu
{
	class MenuToolButtonSubmenuWidget : public MenuToolButtonWidget
	{
		VTX_WIDGET

	  private:
	  public:
		void addAction( QAction * const p_action );

		template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
		void addAction( const std::string & p_name,
						const W * const		p_receiver,
						void ( W::*p_action )() const,
						const bool p_default = false )
		{
			UIAction::SelfReferencedAction * const action
				= new UIAction::SelfReferencedAction( QString::fromStdString( p_name ), _submenu );

			p_receiver->connect( action, &QAction::triggered, p_receiver, p_action );
			connect( action,
					 &UIAction::SelfReferencedAction::triggeredSelf,
					 this,
					 &MenuToolButtonSubmenuWidget::_actionHasBeenTriggered );

			_submenu->addAction( action );

			if ( p_default || _defaultAction == nullptr )
				_setDefaultAction( action );
		}

		template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
		void addAction( const std::string & p_name,
						const W * const		p_receiver,
						void ( W::*p_action )(),
						const bool p_default = false )
		{
			UIAction::SelfReferencedAction * const action
				= new UIAction::SelfReferencedAction( QString::fromStdString( p_name ), _submenu );

			p_receiver->connect( action, &QAction::triggered, p_receiver, p_action );
			connect( action,
					 &UIAction::SelfReferencedAction::triggeredSelf,
					 this,
					 &MenuToolButtonSubmenuWidget::_actionHasBeenTriggered );

			_submenu->addAction( action );

			if ( p_default || _defaultAction == nullptr )
				_setDefaultAction( action );
		}

		void setDefaultAction( QAction * const p_defaultAction );
		void setSubmenu( QMenu * const p_submenu );

		void localize() override;

	  protected:
		MenuToolButtonSubmenuWidget( QWidget * p_parent ) : MenuToolButtonWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _setDefaultAction( QAction * const p_action );

		void _triggerDefaultAction() const;
		void _actionHasBeenTriggered( UIAction::SelfReferencedAction * const p_action );

	  private:
		QMenu *	  _submenu		 = nullptr;
		QAction * _defaultAction = nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
