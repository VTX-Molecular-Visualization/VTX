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

		_addSphereWidgetInLayout();
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
