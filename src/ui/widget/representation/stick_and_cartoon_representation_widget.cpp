#include "stick_and_cartoon_representation_widget.hpp"
#include "setting.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void StickAndCartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addCylinderWidgetInLayout( "Stick radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
		_addSSColorModeInLayout( "Cartoon color mode" );
	};

	void StickAndCartoonRepresentationWidget::_refresh()
	{
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );
		_refreshColorModeWidget();
		_refreshSSColorModeWidget();
	}

	void StickAndCartoonRepresentationWidget::localize() {};
} // namespace VTX::UI::Widget::Representation
