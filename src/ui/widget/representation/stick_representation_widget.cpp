#include "stick_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void StickRepresentationWidget::localize() {};
	void StickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
	};

	void StickRepresentationWidget::_refresh()
	{
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );
		_refreshColorModeWidget();
	}

	void StickRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addCylinderValue( p_representation.getCylinderData()._radius );
		_addColorModeValue( p_representation );
	}

} // namespace VTX::UI::Widget::Representation
