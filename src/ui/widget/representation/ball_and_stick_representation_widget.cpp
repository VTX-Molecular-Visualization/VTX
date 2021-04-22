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
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusFixed,
						 _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius,
						   _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );

		_refreshColorModeWidget();
	}

	void BallAndStickRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addSphereValue( p_representation.getSphereData()._radiusFixed,
						 p_representation.isMemberOverrided( MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
		_addCylinderValue( p_representation.getCylinderData()._radius,
						   p_representation.isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );
		_addColorModeValue( p_representation );
	}

	void BallAndStickRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}

	void BallAndStickRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}

} // namespace VTX::UI::Widget::Representation
