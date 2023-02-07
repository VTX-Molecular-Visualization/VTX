#ifndef __VTX_UI_QT_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__

#include "core/main_menu/main_menu_toolbutton.hpp"
#include "qt/widget/base_manual_widget.hpp"
#include <QToolButton>
#include <QWidget>
#include <old/ui/navigation/arrow_navigation.hpp>
#include <type_traits>

namespace VTX::UI::QT::Widget::MainMenu
{
	class MenuToolButtonWidget : public UI::Core::MainMenu::MainMenuToolButton, public BaseManualWidget<QToolButton>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void setData( const QString & p_name, const QString & p_iconUrl, const Qt::Orientation p_orientation );
		void setData( const QString & p_name, const QIcon & p_icon, const Qt::Orientation p_orientation );
		void setName( const QString & p_name );
		void setOrientation( const Qt::Orientation p_orientation );

		template<typename F, typename = std::enable_if<std::is_base_of<QWidget, F>::value>>
		void setTriggerAction( const F * const p_receiver, void ( F::*p_action )() const )
		{
			p_receiver->connect( this, &QToolButton::clicked, p_receiver, p_action );
		}
		template<typename F, typename = std::enable_if<std::is_base_of<QWidget, F>::value>>
		void setTriggerAction( const F * const p_receiver, void ( F::*p_action )() )
		{
			p_receiver->connect( this, &QToolButton::clicked, p_receiver, p_action );
		}
		void localize() override;
		void showActiveFeedback( const bool p_activate ) override;

		const Navigation::ArrowNavigation arrowNavigation = Navigation::ArrowNavigation();

	  protected:
		MenuToolButtonWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void keyPressEvent( QKeyEvent * p_event ) override;

		void _updateButtonStyle()
		{
			const Qt::Orientation currOrientation
				= width() > height() ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;
			_updateButtonStyle( currOrientation );
		}
		virtual void _updateButtonStyle( const Qt::Orientation p_orientation );

	  private:
		bool _hasActiveFeedback = false;
	};
} // namespace VTX::UI::QT::Widget::MainMenu
#endif
