#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__

#include "ui/navigation/arrow_navigation.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QToolButton>
#include <QWidget>
#include <type_traits>

namespace VTX::UI::Widget::MainMenu
{
	class MenuToolButtonWidget : public BaseManualWidget<QToolButton>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		void setData( const QString & p_name, const QString & p_iconUrl, const Qt::Orientation p_orientation );
		void setData( const QString & p_name, const QIcon & p_icon, const Qt::Orientation p_orientation );
		void setName( const QString & p_name );
		void setOrientation( const Qt::Orientation p_orientation );

		void setExtraData( const QVariant & p_extraData );

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

		template<typename F, typename = std::enable_if<std::is_base_of<QWidget, F>::value>>
		void setTriggerAction( const F * const p_receiver, void ( F::*p_action )( const QVariant & ) )
		{
			p_receiver->connect(
				this, QOverload<const QVariant &>::of( &MenuToolButtonWidget::triggered ), p_receiver, p_action );
		}
		template<typename F, typename = std::enable_if<std::is_base_of<QWidget, F>::value>>
		void setTriggerAction( const F * const p_receiver, void ( F::*p_action )( const QVariant & ) const )
		{
			p_receiver->connect(
				this, QOverload<const QVariant &>::of( &MenuToolButtonWidget::triggered ), p_receiver, p_action );
		}
		void localize() override;
		void showActiveFeedback( const bool p_activate );

		const Navigation::ArrowNavigation arrowNavigation = Navigation::ArrowNavigation();

	  signals:
		void triggered( const QVariant & p_extraData );

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

		void _triggerActionWithExtraData() { emit triggered( _extraData ); }

	  private:
		bool	 _hasActiveFeedback = false;
		QVariant _extraData			= QVariant();
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
