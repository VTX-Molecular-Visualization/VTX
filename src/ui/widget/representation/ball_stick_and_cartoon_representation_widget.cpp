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

		_addSphereWidgetInLayout( "Balls radius", Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );
		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
		_addSSColorModeInLayout( "Cartoon color mode" );
	};

	void BallStickAndCartoonRepresentationWidget::_refresh()
	{
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusFixed );
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );
		_refreshColorModeWidget();
		_refreshSSColorModeWidget();
	}

	void BallStickAndCartoonRepresentationWidget::localize() {};
} // namespace VTX::UI::Widget::Representation
