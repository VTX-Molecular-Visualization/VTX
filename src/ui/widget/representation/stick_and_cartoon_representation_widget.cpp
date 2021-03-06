#include "stick_and_cartoon_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>

namespace VTX::UI::Widget::Representation
{
	void StickAndCartoonRepresentationWidget::localize() {};
	void StickAndCartoonRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationWidget::_setupUi( p_name );
		_label = new QLabel( this );
		_label->setText( "Stick radius" );

		_stickRadius = VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			this, "stick_radius" );
		_stickRadius->setMinMax( Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		QHBoxLayout * layout = new QHBoxLayout( this );
		layout->addWidget( _label );
		layout->addWidget( _stickRadius );
	};
	void StickAndCartoonRepresentationWidget::_setupSlots()
	{
		connect( _stickRadius,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &StickAndCartoonRepresentationWidget::_onRadiusChange );
	};

	void StickAndCartoonRepresentationWidget::refresh()
	{
		_stickRadius->setValue( _instantiatedRepresentation->getCylinderData()._radius );
	}

	void StickAndCartoonRepresentationWidget::_onRadiusChange( const float p_newRadius )
	{
		VTX_ACTION(
			new Action::ChangeRepresentationCylinderAndSphereRadius( _instantiatedRepresentation, p_newRadius ) );

		emit onDataChange();
	}

} // namespace VTX::UI::Widget::Representation
