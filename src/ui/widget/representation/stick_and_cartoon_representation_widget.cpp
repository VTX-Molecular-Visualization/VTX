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
	void StickAndCartoonRepresentationWidget::localize() {};

	void StickAndCartoonRepresentationWidget::_refresh()
	{
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius,
						   _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );
		_refreshColorModeWidget();
		_refreshSSColorModeWidget();
	}

	void StickAndCartoonRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addCylinderValue( p_representation.getCylinderData()._radius,
						   p_representation.isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );
		_addColorModeValue( p_representation );
		_addSSColorModeValue( p_representation );
	}

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
