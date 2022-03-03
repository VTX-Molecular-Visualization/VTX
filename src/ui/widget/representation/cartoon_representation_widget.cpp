#include "cartoon_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	void CartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_addRibbonWidgetInLayout();
	}

	void CartoonRepresentationWidget::localize() {}
} // namespace VTX::UI::Widget::Representation
