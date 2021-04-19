#include "vdw_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void VdwRepresentationWidget::localize() {};
	void VdwRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );

		_addSphereWidgetInLayout( "Atom offset",
								  Setting::ATOMS_RADIUS_ADD_MIN,
								  Setting::ATOMS_RADIUS_ADD_MAX,
								  Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_ADD );
		_addColorModeInLayout( "Color mode" );
	};

	void VdwRepresentationWidget::_refresh()
	{
		_setSphereValue( _instantiatedRepresentation->getSphereData()._radiusAdd,
						 _instantiatedRepresentation->isMemberOverrided( MEMBER_FLAG::SPHERE_RADIUS_ADD ) );
		_refreshColorModeWidget();
	}

	void VdwRepresentationWidget::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		BaseRepresentationWidget::updateWithNewValue( p_representation );

		_addSphereValue( p_representation.getSphereData()._radiusAdd,
						 p_representation.isMemberOverrided( MEMBER_FLAG::SPHERE_RADIUS_ADD ) );
		_addColorModeValue( p_representation );
	}

} // namespace VTX::UI::Widget::Representation
