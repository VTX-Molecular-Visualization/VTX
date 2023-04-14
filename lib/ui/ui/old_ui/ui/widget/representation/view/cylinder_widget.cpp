#include "cylinder_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	void CylinderWidget::_setupUi( const QString & p_name )
	{
		BaseRepresentationViewWidget::_setupUi( p_name );

		_radiusLabel = new QLabel( this );
		_radiusLabel->setText( "Sticks radius" );
		_radiusWidget = _addFloatFieldSliderWidgetInLayout(
			"cylinderRadiusWidget", _radiusLabel, Setting::BONDS_RADIUS_MIN, Setting::BONDS_RADIUS_MAX );

		_colorBlendingModeLabel = new QLabel( this );
		_colorBlendingModeLabel->setText( "Color blending mode" );
		_colorBlendingModeWidget = _addQComboBoxMultiFieldWidgetInLayout(
			"cylinderColorBLendingModeWidget", _colorBlendingModeLabel, Generic::COLOR_BLENDING_MODE_STRING );
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
		emit onColorBlendingModeChange( Generic::COLOR_BLENDING_MODE( p_newMode ) );
	}

	void CylinderWidget::refresh( const Model::Representation::InstantiatedRepresentation &		 p_representation,
								  const std::unordered_set<const Generic::BaseRepresentable *> & p_targets )
	{
		Util::UI::setDynamicProperty(
			_radiusLabel,
			Style::WidgetProperty::OVERIDDEN_PARAMETER,
			p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS ) );
		Util::UI::setDynamicProperty(
			_colorBlendingModeLabel,
			Style::WidgetProperty::OVERIDDEN_PARAMETER,
			p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_COLOR_BLENDING_MODE ) );

		_radiusWidget->setValue( p_representation.getCylinderData().radius );
		_colorBlendingModeWidget->setCurrentIndex( int( p_representation.getCylinderData().colorBlendingMode ) );
	}

	void CylinderWidget::updateWithNewValue( const Model::Representation::InstantiatedRepresentation & p_representation,
											 const std::unordered_set<const Generic::BaseRepresentable *> & p_targets )
	{
		_updateLabelOverriddenProperty(
			_radiusLabel, p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS ) );
		_updateLabelOverriddenProperty(
			_colorBlendingModeLabel,
			p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_COLOR_BLENDING_MODE ) );

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
