#include "sas_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	void SasRepresentationWidget::localize() {}

	void SasRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_addColorModeWidgetInLayout();
	}

} // namespace VTX::UI::Widget::Representation
