#include "ui/old_ui/ui/widget/settings/render_effect_preset_editor.hpp"
#include "ui/old_ui/action/renderer.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <app/action/renderer.hpp>
#include <app/application/setting.hpp>
#include <app/core/view/callback_view.hpp>

namespace VTX::UI::Widget::Settings
{
	RenderEffectPresetEditor::RenderEffectPresetEditor( QWidget * const p_parent ) :
		VTX::UI::Widget::BaseManualWidget<QScrollArea>( p_parent ),
		EditorView( ID::View::UI_REPRESENTATION_EDITOR_PRESET )
	{
	}

	void RenderEffectPresetEditor::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const viewport = new QWidget( this );

		_name = WidgetFactory::get().instantiateWidget<CustomWidget::FilenameFieldWidget>( viewport, "presetName" );

		_quickAccess = new QCheckBox( viewport );

		_shading = new QComboBox( viewport );
		_shading->addItems( SHADING );

		_enableSSAO = new QCheckBox( viewport );

		_ssaoIntensity
			= VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldSliderWidget>( viewport, "ssaoIntensity" );
		_ssaoIntensity->setMinMax( VTX::App::Application::Setting::AO_INTENSITY_MIN, VTX::App::Application::Setting::AO_INTENSITY_MAX );
		_ssaoBlurSize
			= VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldSliderWidget>( viewport, "ssaoBlurSize" );
		_ssaoBlurSize->setMinMax( VTX::App::Application::Setting::AO_BLUR_SIZE_MIN, VTX::App::Application::Setting::AO_BLUR_SIZE_MAX );

		_enableOutline = new QCheckBox( viewport );
		_outlineThickness
			= VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldSliderWidget>( viewport, "outlineThickness" );
		_outlineThickness->setMinMax( VTX::App::Application::Setting::OUTLINE_THICKNESS_MIN, VTX::App::Application::Setting::OUTLINE_THICKNESS_MAX );
		_outlineSensivity
			= VTX::UI::WidgetFactory::get().instantiateWidget<FloatFieldSliderWidget>( viewport, "outlineSensivity" );
		_outlineSensivity->setMinMax( VTX::App::Application::Setting::OUTLINE_SENSIVITY_MIN, VTX::App::Application::Setting::OUTLINE_SENSIVITY_MAX );
		_outlineColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( viewport, "outlineColor" );

		_enableFog = new QCheckBox( viewport );
		_nearFog = VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldDraggableWidget>( viewport, "nearFog" );
		_nearFog->setMinMax( VTX::App::Application::Setting::FOG_NEAR_MIN, VTX::App::Application::Setting::FOG_NEAR_MAX );
		_nearFog->setLabel( "Near" );
		_farFog = VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldDraggableWidget>( viewport, "farFog" );
		_farFog->setMinMax( VTX::App::Application::Setting::FOG_FAR_MIN, VTX::App::Application::Setting::FOG_FAR_MAX );
		_farFog->setLabel( "Far" );
		_fogDensity = VTX::UI::WidgetFactory::get().instantiateWidget<FloatFieldSliderWidget>( viewport, "fogDensity" );
		_fogDensity->setMinMax( 0.0f, 1.0f );
		_fogColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( viewport, "fogColor" );

		_backgroundColor
			= VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( viewport, "backgroundColor" );
		_cameraLightColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( viewport, "lightColor" );

		_attributeLayout = new Layout::AttributeListLayout( viewport );
		_attributeLayout->setEndSectionSpacerSize( Style::ATTRIBUTE_LIST_LAYOUT_SMALL_SECTION_SPACER );

		_attributeLayout->startAttributeSection( "Description" );
		_attributeLayout->addAttribute( _name, "Name" );
		_attributeLayout->addAttribute( _quickAccess, "QuickAccess" );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "Shading" );
		_attributeLayout->addAttribute( _shading, "Shading" );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "SSAO", _enableSSAO );
		_attributeLayout->addAttribute( _ssaoIntensity, QString( "Intensity" ) );
		_attributeLayout->addAttribute( _ssaoBlurSize, QString( "Blur Size" ) );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "Outline", _enableOutline );
		_attributeLayout->addAttribute( _outlineThickness, QString( "Thickness" ) );
		_attributeLayout->addAttribute( _outlineSensivity, QString( "Sensivity" ) );
		_attributeLayout->addAttribute( _outlineColor, QString( "Color" ) );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "Fog", _enableFog );
		_attributeLayout->addAttribute( _nearFog, _nearFog->getLabelWidget() );
		_attributeLayout->addAttribute( _farFog, _farFog->getLabelWidget() );
		_attributeLayout->addAttribute( _fogDensity, "Density" );
		_attributeLayout->addAttribute( _fogColor, "Color" );
		_attributeLayout->finishAttributeSection();

		_attributeLayout->startAttributeSection( "Camera" );
		_attributeLayout->addAttribute( _backgroundColor, "Background color" );
		_attributeLayout->addAttribute( _cameraLightColor, "Light Color" );
		_attributeLayout->finishAttributeSection( false );

		setWidget( viewport );
	}
	void RenderEffectPresetEditor::_setupSlots()
	{
		connect( _name,
				 &CustomWidget::FilenameFieldWidget::editingFinished,
				 this,
				 &RenderEffectPresetEditor::_onNameChanged );
		connect( _quickAccess, &QCheckBox::clicked, this, &RenderEffectPresetEditor::_onQuickAccessChanged );

		connect( _shading,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RenderEffectPresetEditor::_onShadingChange );

		connect( _enableSSAO,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOStateChanged );
		connect( _ssaoIntensity,
				 QOverload<int>::of( &IntegerFieldSliderWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOIntensityChanged );
		connect( _ssaoBlurSize,
				 QOverload<int>::of( &IntegerFieldSliderWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOBlurSizeChanged );

		connect( _enableOutline,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineStateChanged );
		connect( _outlineThickness,
				 &IntegerFieldSliderWidget::onValueChange,
				 this,
				 &RenderEffectPresetEditor::_onOutlineThicknessChanged );
		connect( _outlineSensivity,
				 QOverload<float>::of( &FloatFieldSliderWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineSensivityChanged );
		connect( _outlineColor,
				 QOverload<const Color::Rgba &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineColorChanged );

		connect( _enableFog,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onFogStateChanged );
		connect(
			_nearFog, &IntegerFieldDraggableWidget::onValueChange, this, &RenderEffectPresetEditor::_onFogNearChanged );
		connect(
			_farFog, &IntegerFieldDraggableWidget::onValueChange, this, &RenderEffectPresetEditor::_onFogFarChanged );
		connect( _fogDensity,
				 QOverload<float>::of( &FloatFieldSliderWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onFogDensityChanged );
		connect( _fogColor,
				 QOverload<const Color::Rgba &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onFogColorChanged );

		connect( _backgroundColor,
				 QOverload<const Color::Rgba &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onBackgroundColorChanged );
		connect( _cameraLightColor,
				 QOverload<const Color::Rgba &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onCameraLightColorChanged );
	}

	void RenderEffectPresetEditor::localize() {}

	void RenderEffectPresetEditor::_catchModelEvent( const VTX::App::Core::Event::VTXEvent * const p_event ) { refresh(); }

	void RenderEffectPresetEditor::refresh()
	{
		_name->setText( QString::fromStdString( _preset->getName() ) );

		_quickAccess->setChecked( _preset->hasQuickAccess() );

		_shading->setCurrentIndex( (int)_preset->getShading() );

		const bool ssaoEnabled = _preset->isSSAOEnabled();
		_enableSSAO->setChecked( ssaoEnabled );
		_ssaoIntensity->setValue( _preset->getSSAOIntensity() );
		_attributeLayout->enableAttribute( _ssaoIntensity, ssaoEnabled );
		_ssaoBlurSize->setValue( _preset->getSSAOBlurSize() );
		_attributeLayout->enableAttribute( _ssaoBlurSize, ssaoEnabled );

		const bool outlineEnabled = _preset->isOutlineEnabled();
		_enableOutline->setChecked( outlineEnabled );
		_outlineThickness->setValue( _preset->getOutlineThickness() );
		_attributeLayout->enableAttribute( _outlineThickness, outlineEnabled );
		_outlineSensivity->setValue( _preset->getOutlineSensivity() );
		_attributeLayout->enableAttribute( _outlineSensivity, outlineEnabled );
		_outlineColor->setColor( _preset->getOutlineColor() );
		_attributeLayout->enableAttribute( _outlineColor, outlineEnabled );

		const bool fogEnabled = _preset->isFogEnabled();
		_enableFog->setChecked( fogEnabled );
		_nearFog->setValue( _preset->getFogNear() );
		_attributeLayout->enableAttribute( _nearFog, fogEnabled );
		_farFog->setValue( _preset->getFogFar() );
		_attributeLayout->enableAttribute( _farFog, fogEnabled );
		_fogDensity->setValue( _preset->getFogDensity() );
		_attributeLayout->enableAttribute( _fogDensity, fogEnabled );
		_fogColor->setColor( _preset->getFogColor() );
		_attributeLayout->enableAttribute( _fogColor, fogEnabled );

		_backgroundColor->setColor( _preset->getBackgroundColor() );
		_cameraLightColor->setColor( _preset->getCameraLightColor() );
	}

	void RenderEffectPresetEditor::setPreset( App::Application::RenderEffect::RenderEffectPreset * const p_model,
											  const bool								  p_updateRender )
	{
		if ( _preset == p_model )
			return;

		_preset = p_model;

		createTempView( _preset );

		blockSignals( !p_updateRender );
		refresh();
		blockSignals( false );
	}

	void RenderEffectPresetEditor::_onNameChanged() const
	{
		const std::string strName = _name->text().toStdString();

		if ( strName == "" )
		{
			_name->setText( QString::fromStdString( _preset->getName() ) );
			return;
		}

		if ( !signalsBlocked() && _preset->getName() != strName )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeName( *_preset, strName ) );
	}
	void RenderEffectPresetEditor::_onQuickAccessChanged( const bool p_quickAccess ) const
	{
		if ( !signalsBlocked() && _preset->hasQuickAccess() != p_quickAccess )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeQuickAccess( *_preset, p_quickAccess ) );
	}

	void RenderEffectPresetEditor::_onShadingChange( const int p_newIndex ) const
	{
		const VTX::Renderer::SHADING shading = VTX::Renderer::SHADING( p_newIndex );
		if ( !signalsBlocked() && shading != _preset->getShading() )
			VTX_ACTION( new VTX::UI::Action::Renderer::ChangeShading( *_preset, shading ) );
	}

	void RenderEffectPresetEditor::_onSSAOStateChanged( const int p_state ) const
	{
		const bool enable = p_state != 0;
		if ( !signalsBlocked() && enable != _preset->isSSAOEnabled() )
			VTX_ACTION( new VTX::UI::Action::Renderer::EnableSSAO( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onSSAOIntensityChanged( const int p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getSSAOIntensity() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeSSAOIntensity( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onSSAOBlurSizeChanged( const int p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getSSAOBlurSize() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeSSAOBlurSize( *_preset, p_value ) );
	}

	void RenderEffectPresetEditor::_onOutlineStateChanged( const int p_state ) const
	{
		const bool enable = p_state != 0;
		if ( !signalsBlocked() && enable != _preset->isOutlineEnabled() )
			VTX_ACTION( new VTX::UI::Action::Renderer::EnableOutline( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onOutlineThicknessChanged( const uint p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getOutlineThickness() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeOutlineThickness( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onOutlineSensivityChanged( const float p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getOutlineSensivity() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeOutlineSensivity( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onOutlineColorChanged( const Color::Rgba & p_color ) const
	{
		if ( !signalsBlocked() && p_color != _preset->getOutlineColor() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeOutlineColor( *_preset, p_color ) );
	}

	void RenderEffectPresetEditor::_onFogStateChanged( const int p_state ) const
	{
		const bool enable = p_state != 0;
		if ( !signalsBlocked() && enable != _preset->isFogEnabled() )
			VTX_ACTION( new VTX::UI::Action::Renderer::EnableFog( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onFogNearChanged( const int p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getFogNear() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeFogNear( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onFogFarChanged( const int p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getFogFar() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeFogFar( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onFogDensityChanged( const float p_value ) const
	{
		if ( !signalsBlocked() && p_value != _preset->getFogDensity() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeFogDensity( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onFogColorChanged( const Color::Rgba & p_color ) const
	{
		if ( !signalsBlocked() && p_color != _preset->getFogColor() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeFogColor( *_preset, p_color ) );
	}
	void RenderEffectPresetEditor::_onBackgroundColorChanged( const Color::Rgba & p_color ) const
	{
		if ( !signalsBlocked() && p_color != _preset->getBackgroundColor() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeBackgroundColor( *_preset, p_color ) );
	}
	void RenderEffectPresetEditor::_onCameraLightColorChanged( const Color::Rgba & p_color ) const
	{
		if ( !signalsBlocked() && p_color != _preset->getCameraLightColor() )
			VTX_ACTION( new VTX::App::Action::Renderer::ChangeCameraLightColor( *_preset, p_color ) );
	}
} // namespace VTX::UI::Widget::Settings
