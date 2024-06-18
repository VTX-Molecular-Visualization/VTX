#ifndef __VTX_UI_QT_WIDGET_BASE_MANUAL_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_MANUAL_WIDGET__

#include "base_manual_widget_initializer.hpp"
#include "concepts.hpp"
#include <type_traits>

namespace VTX::UI::QT::Core
{
	template<QTWidgetConcept W>
	class BaseManualWidget : public W, public BaseManualWidgetInitializer
	{
		NEW_ARCHI_VTX_WIDGET

	  protected:
		BaseManualWidget( QWidget * p_parent = nullptr ) : W( p_parent ), BaseManualWidgetInitializer() {};

		virtual void _setupUi( const QString & p_name ) override { W::setObjectName( p_name ); }
	};
} // namespace VTX::UI::QT::Core

#endif
