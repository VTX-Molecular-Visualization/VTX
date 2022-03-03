#include "stick_and_cartoon_representation_widget.hpp"
#include "setting.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void StickAndCartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addCylinderWidgetInLayout();
		_addColorModeWidgetInLayout();
		_addRibbonWidgetInLayout();
	}

	void StickAndCartoonRepresentationWidget::localize() {}

	void StickAndCartoonRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}

} // namespace VTX::UI::Widget::Representation
