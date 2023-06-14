#include "ui/old_ui/ui/widget/representation/stick_representation_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <app/old/action/representable.hpp>
#include <app/old/application/setting.hpp>

namespace VTX::UI::Widget::Representation
{
	void StickRepresentationWidget::localize() {};
	void StickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addCylinderWidgetInLayout();
		_addColorModeWidgetInLayout();
	}

	void StickRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange( App::Old::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS
						   | App::Old::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED );
	}

} // namespace VTX::UI::Widget::Representation
