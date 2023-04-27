#include "ui/old_ui/ui/widget/representation/ball_and_stick_representation_widget.hpp"
#include <app/old_app/setting.hpp>

namespace VTX::UI::Widget::Representation
{
	void BallAndStickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout();
		_sphereWidget->setRadiusOffsetWidgetVisible( false );
		_addCylinderWidgetInLayout();
		_addColorModeWidgetInLayout();
	}

	void BallAndStickRepresentationWidget::localize() {}

	void BallAndStickRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange( App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS
						   | App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED );
	}

	void BallAndStickRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange( App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS
						   | App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED );
	}

} // namespace VTX::UI::Widget::Representation
