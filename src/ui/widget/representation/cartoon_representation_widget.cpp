#include "cartoon_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	void CartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_addSSColorModeInLayout( "Cartoon color mode" );
	};
	void CartoonRepresentationWidget::localize() {};

	void CartoonRepresentationWidget::_refresh() { _refreshSSColorModeWidget(); }

	void CartoonRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );
		_addSSColorModeValue( p_representation );
	}
} // namespace VTX::UI::Widget::Representation
