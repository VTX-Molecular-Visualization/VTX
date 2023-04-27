#include "ui/old_ui/ui/widget/representation/ball_stick_and_cartoon_representation_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QGridLayout>
#include <QLabel>
#include <app/action/instantiated_representation.hpp>
#include <app/old_app/setting.hpp>

namespace VTX::UI::Widget::Representation
{
	void BallStickAndCartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout();
		_sphereWidget->setRadiusOffsetWidgetVisible( false );
		_addCylinderWidgetInLayout();
		_addColorModeWidgetInLayout();
		_addRibbonWidgetInLayout();
	}

	void BallStickAndCartoonRepresentationWidget::localize() {}

	void BallStickAndCartoonRepresentationWidget::_onSphereRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setSphereRadius( p_newRadius );

		emit onDataChange( App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS
						   | App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED );
	}
	void BallStickAndCartoonRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange( App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS
						   | App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED );
	}

} // namespace VTX::UI::Widget::Representation
