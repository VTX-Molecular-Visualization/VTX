#include "ui/old_ui/ui/widget/representation/view/sphere_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	void SphereWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationViewWidget::_setupUi( p_name );

		_radiusLabel = new QLabel( this );
		_radiusLabel->setText( "Balls radius" );
		_radiusWidget = _addFloatFieldSliderWidgetInLayout(
			"sphereRadiusWidget", _radiusLabel, VTX::App::Application::Setting::ATOMS_RADIUS_MIN, VTX::App::Application::Setting::ATOMS_RADIUS_MAX );

		_radiusOffsetLabel = new QLabel( this );
		_radiusOffsetLabel->setText( "Atom offset" );
		_radiusOffsetWidget = _addFloatFieldSliderWidgetInLayout( "sphereRadiusOffsetWidget",
																  _radiusOffsetLabel,
																  VTX::App::Application::Setting::ATOMS_RADIUS_ADD_MIN,
																  VTX::App::Application::Setting::ATOMS_RADIUS_ADD_MAX );
	}

	void SphereWidget::_setupSlots()
	{
		connect(
			_radiusWidget, &CustomWidget::FloatFieldSliderWidget::onValueChange, this, &SphereWidget::_onRadiusChange );
		connect( _radiusOffsetWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &SphereWidget::_onRadiusOffsetChange );
	}

	void SphereWidget::_onRadiusChange( const float p_newRadius ) { emit onRadiusChange( p_newRadius ); }

	void SphereWidget::_onRadiusOffsetChange( const float p_newRadius ) { emit onRadiusOffsetChange( p_newRadius ); }

	void SphereWidget::refresh(
		const App::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets )
	{
		Util::UI::setDynamicProperty( _radiusLabel,
									  Style::WidgetProperty::OVERIDDEN_PARAMETER,
									  p_representation.isMemberOverrided(
										  App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED ) );
		Util::UI::setDynamicProperty( _radiusOffsetLabel,
									  Style::WidgetProperty::OVERIDDEN_PARAMETER,
									  p_representation.isMemberOverrided(
										  App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_ADD ) );

		_radiusWidget->setValue( p_representation.getSphereData().radiusFixed );
		_radiusOffsetWidget->setValue( p_representation.getSphereData().radiusAdd );
	}

	void SphereWidget::updateWithNewValue(
		const App::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets )
	{
		_updateLabelOverriddenProperty(
			_radiusLabel,
			p_representation.isMemberOverrided(
				App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_FIXED ) );
		_updateLabelOverriddenProperty( _radiusOffsetLabel,
										p_representation.isMemberOverrided(
											App::Application::Representation::MEMBER_FLAG::ENUM::SPHERE_RADIUS_ADD ) );

		_radiusWidget->updateWithNewValue( p_representation.getSphereData().radiusFixed );
		_radiusOffsetWidget->updateWithNewValue( p_representation.getSphereData().radiusAdd );
	}

	void SphereWidget::resetState()
	{
		Util::UI::setDynamicProperty( _radiusLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );
		Util::UI::setDynamicProperty( _radiusOffsetLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );

		_radiusWidget->resetState();
		_radiusOffsetWidget->resetState();
	}

	void SphereWidget::setRadiusWidgetVisible( const bool p_visible )
	{
		_radiusWidget->setDisabled( !p_visible );
		_radiusWidget->setVisible( p_visible );
		_radiusLabel->setVisible( p_visible );
	}

	void SphereWidget::setRadiusOffsetWidgetVisible( const bool p_visible )
	{
		_radiusOffsetWidget->setDisabled( !p_visible );
		_radiusOffsetWidget->setVisible( p_visible );
		_radiusOffsetLabel->setVisible( p_visible );
	}

} // namespace VTX::UI::Widget::Representation::View
