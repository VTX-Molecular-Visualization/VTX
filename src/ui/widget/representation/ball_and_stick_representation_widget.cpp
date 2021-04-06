#include "ball_and_stick_representation_widget.hpp"
#include "setting.hpp"

namespace VTX::UI::Widget::Representation
{
	void BallAndStickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout( "Balls radius",
								  Setting::ATOMS_RADIUS_MIN,
								  Setting::ATOMS_RADIUS_MAX,
								  Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED );
		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
	};
	void BallAndStickRepresentationWidget::localize() {};

	void BallAndStickRepresentationWidget::_refresh()
	{
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusFixed );
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );

		_refreshColorModeWidget();
	}

	void BallAndStickRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addSphereValue( p_representation.getSphereData()._radiusFixed );
		_addCylinderValue( p_representation.getCylinderData()._radius );
		_addColorModeValue( p_representation );
	}
} // namespace VTX::UI::Widget::Representation
