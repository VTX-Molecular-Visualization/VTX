#include "ball_and_stick_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/instantiated_representation.hpp"
#include "action/molecule.hpp"
#include "setting.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <string>

namespace VTX::UI::Widget::Representation
{
	void BallAndStickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout( "Balls radius", Setting::ATOMS_RADIUS_MIN, Setting::ATOMS_RADIUS_MAX );
		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );
	};

	void BallAndStickRepresentationWidget::_refresh()
	{
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusFixed );
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius );

		_refreshColorModeWidget();
	}

	void BallAndStickRepresentationWidget::localize() {};
} // namespace VTX::UI::Widget::Representation
