#include "ball_stick_and_cartoon_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <QLabel>

namespace VTX::UI::Widget::Representation
{
	void BallStickAndCartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout( "Balls radius",
								  Setting::ATOMS_RADIUS_MIN,
								  Setting::ATOMS_RADIUS_MAX,
								  Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED );
		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
		_addSSColorModeInLayout( "Cartoon color mode" );
	};
	void BallStickAndCartoonRepresentationWidget::localize() {};

	void BallStickAndCartoonRepresentationWidget::_refresh()
	{
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusFixed );
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );
		_refreshColorModeWidget();
		_refreshSSColorModeWidget();
	}

	void BallStickAndCartoonRepresentationWidget::updateWithNewValue(
		const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addSphereValue( p_representation.getSphereData()._radiusFixed );
		_addCylinderValue( p_representation.getCylinderData()._radius );
		_addColorModeValue( p_representation );
		_addSSColorModeValue( p_representation );
	}

	void BallStickAndCartoonRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}
	void BallStickAndCartoonRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}

} // namespace VTX::UI::Widget::Representation
