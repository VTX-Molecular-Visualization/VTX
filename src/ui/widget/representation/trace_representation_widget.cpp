#include "trace_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void TraceRepresentationWidget::localize() {};
	void TraceRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
	};
	void TraceRepresentationWidget::_setupSlots() {};

	void TraceRepresentationWidget::refresh() {}

} // namespace VTX::UI::Widget::Representation
