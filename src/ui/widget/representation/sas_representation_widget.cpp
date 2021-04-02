#include "sas_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	void SasRepresentationWidget::localize() {};
	void SasRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_addColorModeInLayout( "Color mode" );
	};

	void SasRepresentationWidget::_refresh() { _refreshColorModeWidget(); }

	void SasRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );
		_addColorModeValue( p_representation );
	}

} // namespace VTX::UI::Widget::Representation
