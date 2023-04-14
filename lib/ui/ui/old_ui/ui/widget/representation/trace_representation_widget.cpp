#include "trace_representation_widget.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/representable.hpp>

namespace VTX::UI::Widget::Representation
{
	void TraceRepresentationWidget::_setupUi( const QString & p_name ) { BaseRepresentationWidget::_setupUi( p_name ); }

	void TraceRepresentationWidget::localize() {}
} // namespace VTX::UI::Widget::Representation
