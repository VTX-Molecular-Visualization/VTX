#ifndef __VTX_UI_QT_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__
#define __VTX_UI_QT_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__

#include <QtCore>
#include <string>

namespace VTX::UI::QT
{
	class WidgetFactory;
}

#define NEW_ARCHI_VTX_WIDGET friend ::VTX::UI::QT::WidgetFactory;

namespace VTX::UI::QT::Widget
{
	class BaseManualWidgetInitializer
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		virtual ~BaseManualWidgetInitializer() = default;

		virtual void localize() = 0;

	  protected:
		BaseManualWidgetInitializer() {};

		inline void _setup( const std::string & p_name ) { _setup( QString::fromStdString( p_name ) ); };
		inline void _setup( const QString & p_name )
		{
			_setupUi( p_name );
			_setupSlots();
			localize();
		};

		virtual void _setupUi( const QString & p_name ) = 0;
		virtual void _setupSlots()						= 0;
	};
} // namespace VTX::UI::QT::Widget

#endif
