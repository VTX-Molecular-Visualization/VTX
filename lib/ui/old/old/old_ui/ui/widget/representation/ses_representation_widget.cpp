#include "ui/old_ui/ui/widget/representation/ses_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	void SesRepresentationWidget::localize() {}

	void SesRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_addColorModeWidgetInLayout();
	}

} // namespace VTX::UI::Widget::Representation
