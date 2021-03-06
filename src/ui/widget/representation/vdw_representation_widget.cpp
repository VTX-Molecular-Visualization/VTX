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

		_label = new QLabel( this );
		_label->setText( "Atom radius" );

		_sphereOffsetRadius = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "sphere_offset_radius" );

		_sphereOffsetRadius->setMinMax( Setting::ATOMS_RADIUS_ADD_MIN, Setting::ATOMS_RADIUS_ADD_MAX );

		QHBoxLayout * layout = new QHBoxLayout( this );
		layout->addWidget( _label );
		layout->addWidget( _sphereOffsetRadius );
	};
	void VdwRepresentationWidget::_setupSlots()
	{
		connect( _sphereOffsetRadius,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &VdwRepresentationWidget::_onRadiusOffsetChange );
	};

	void VdwRepresentationWidget::refresh()
	{
		_sphereOffsetRadius->setValue( _instantiatedRepresentation->getSphereData()._radiusAdd );
	}

	void VdwRepresentationWidget::_onRadiusOffsetChange( const float p_newRadius )
	{
		VTX_ACTION( new Action::ChangeRepresentationSphereRadiusAdd( _instantiatedRepresentation, p_newRadius ) );
		emit onDataChange();
	}

} // namespace VTX::UI::Widget::Representation
