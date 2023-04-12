#ifndef __VTX_UI_WIDGET_TRACE_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_TRACE_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Representation
{
	class TraceRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		TraceRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
