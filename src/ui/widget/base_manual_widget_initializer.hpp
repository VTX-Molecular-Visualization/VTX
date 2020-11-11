#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__

#ifdef _MSC_VER
#pragma once
#endif

#include <QtCore>
#include <string>

namespace VTX
{
	namespace UI
	{
		class WidgetFactory;
	}
} // namespace VTX

#define VTX_MANUAL_WIDGET_DECLARATION friend ::VTX::UI::WidgetFactory;

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class BaseManualWidgetInitializer
			{
				VTX_MANUAL_WIDGET_DECLARATION

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
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
