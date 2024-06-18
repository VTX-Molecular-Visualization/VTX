#ifndef __VTX_UI_QT_WIDGETFACTORY__
#define __VTX_UI_QT_WIDGETFACTORY__

#include "core/base_manual_widget.hpp"
#include "core/base_manual_widget_initializer.hpp"
#include <QMainWindow>
#include <QTreeWidget>
#include <QWidget>
#include <string>
#include <type_traits>

namespace VTX::UI::QT
{
	class WidgetFactory
	{
	  public:
		inline static WidgetFactory & get()
		{
			static WidgetFactory instance;
			return instance;
		}

	  public:
		template<typename W, typename = std::enable_if<std::is_base_of<Core::BaseManualWidgetInitializer, W>::value>>
		W * const instantiateWidget( QWidget * const p_parent, const std::string & p_name ) const
		{
			W * const res = new W( p_parent );

			_initManualWidget<W>( res, p_name );

			return res;
		}

		template<
			typename W,
			typename P1,
			typename = std::enable_if<std::is_base_of<Core::BaseManualWidgetInitializer, W>::value>>
		W * const instantiateWidget( QWidget * const p_parent, P1 p_param1, const std::string & p_name ) const
		{
			// Parent can be optional in most of widget, so we reference it at the end of the params
			W * const res = new W( p_param1, p_parent );

			_initManualWidget<W>( res, p_name );

			return res;
		}

	  private:
		WidgetFactory() = default;

		template<typename W>
		void _initManualWidget( W * const p_widget, const std::string & p_name ) const
		{
			const QString qstringName = QString::fromStdString( p_name );

			p_widget->blockSignals( true );
			p_widget->setUpdatesEnabled( false );
			p_widget->_setup( qstringName );
			p_widget->setUpdatesEnabled( true );
			p_widget->blockSignals( false );
		}
	};
} // namespace VTX::UI::QT

#endif
