#include "ui/old_ui/ui/widget/representation/view/cylinder_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	void CylinderWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationViewWidget::_setupUi( p_name );

		_radiusLabel = new QLabel( this );
		_radiusLabel->setText( "Sticks radius" );
		_radiusWidget = _addFloatFieldSliderWidgetInLayout(
			"cylinderRadiusWidget", _radiusLabel, VTX::App::Application::Setting::BONDS_RADIUS_MIN, VTX::App::Application::Setting::BONDS_RADIUS_MAX );

		_colorBlendingModeLabel = new QLabel( this );
		_colorBlendingModeLabel->setText( "Color blending mode" );
		_colorBlendingModeWidget = _addQComboBoxMultiFieldWidgetInLayout(
			"cylinderColorBLendingModeWidget", _colorBlendingModeLabel, App::Internal::ChemDB::Color::COLOR_BLENDING_MODE_STRING );
	}

	void CylinderWidget::_setupSlots()
	{
		connect( _radiusWidget,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &CylinderWidget::_onRadiusChange );
		connect( _colorBlendingModeWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &CylinderWidget::_onColorBlendingModeChange );
	}

	void CylinderWidget::_onRadiusChange( const float p_newRadius ) { emit onRadiusChange( p_newRadius ); }

	void CylinderWidget::_onColorBlendingModeChange( const int p_newMode )
	{
		emit onColorBlendingModeChange( App::Internal::ChemDB::Color::COLOR_BLENDING_MODE( p_newMode ) );
	}

	void CylinderWidget::refresh(
		const App::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets )
	{
		const bool radiusOverrided = p_representation.isMemberOverrided(
			App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS );

		const bool colorBlendingModeOverrided = p_representation.isMemberOverrided(
			App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_COLOR_BLENDING_MODE );

		Util::UI::setDynamicProperty( _radiusLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, radiusOverrided );
		Util::UI::setDynamicProperty(
			_colorBlendingModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, colorBlendingModeOverrided );

		_radiusWidget->setValue( p_representation.getCylinderData().radius );
		_colorBlendingModeWidget->setCurrentIndex( int( p_representation.getCylinderData().colorBlendingMode ) );
	}

	void CylinderWidget::updateWithNewValue(
		const App::Application::Representation::InstantiatedRepresentation &					p_representation,
		const std::unordered_set<const App::Application::Representation::BaseRepresentable *> & p_targets )
	{
		_updateLabelOverriddenProperty( _radiusLabel,
										p_representation.isMemberOverrided(
											App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_RADIUS ) );
		_updateLabelOverriddenProperty(
			_colorBlendingModeLabel,
			p_representation.isMemberOverrided(
				App::Application::Representation::MEMBER_FLAG::ENUM::CYLINDER_COLOR_BLENDING_MODE ) );

		_radiusWidget->updateWithNewValue( p_representation.getCylinderData().radius );
		_colorBlendingModeWidget->updateWithNewValue( int( p_representation.getCylinderData().colorBlendingMode ) );
	}

	void CylinderWidget::resetState()
	{
		Util::UI::setDynamicProperty( _radiusLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );
		Util::UI::setDynamicProperty( _colorBlendingModeLabel, Style::WidgetProperty::OVERIDDEN_PARAMETER, true );

		_radiusWidget->resetState();
		_colorBlendingModeWidget->resetState();
	}
} // namespace VTX::UI::Widget::Representation::View
