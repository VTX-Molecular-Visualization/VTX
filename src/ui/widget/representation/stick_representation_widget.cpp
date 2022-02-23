#include "stick_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void StickRepresentationWidget::localize() {};
	void StickRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addCylinderWidgetInLayout( "Sticks radius", Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );
		_addColorModeInLayout( "Color mode" );

		_addTransitionColorModeInLayout( "Transition color mode" );
		QStringList colorModeList = QStringList();
		for ( const std::string colorModeStrings : Generic::TRANSITION_COLOR_MODE_STRING )
		{
			colorModeList.append( QString::fromStdString( colorModeStrings ) );
		}
		_transitionColorModeWidget->addItems( colorModeList );
	};

	void StickRepresentationWidget::_refresh()
	{
		_setCylinderValue( _instantiatedRepresentation->getCylinderData()._radius,
						   _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );

		_refreshColorModeWidget();
	}

	void StickRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addCylinderValue( p_representation.getCylinderData()._radius,
						   p_representation.isMemberOverrided( MEMBER_FLAG::CYLINDER_RADIUS ) );

		_addColorModeValue( p_representation );
	}

	void StickRepresentationWidget::_onCylinderRadiusChange( const float p_newRadius )
	{
		if ( signalsBlocked() )
			return;

		_instantiatedRepresentation->setCylinderRadius( p_newRadius );

		emit onDataChange(
			Model::Representation::MEMBER_FLAG( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS
												| Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) );
	}

} // namespace VTX::UI::Widget::Representation
