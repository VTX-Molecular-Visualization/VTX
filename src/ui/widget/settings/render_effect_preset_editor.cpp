#include "render_effect_preset_editor.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "setting.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Settings
{
	RenderEffectPresetEditor::RenderEffectPresetEditor( QWidget * const p_parent ) :
		VTX::UI::Widget::BaseManualWidget<QScrollArea>( p_parent )
	{
	}

	void RenderEffectPresetEditor::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_shading = new QComboBox( this );
		_shading->addItems( SHADING );

		_enableSSAO = new QCheckBox( this );

		_ssaoIntensity = VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldWidget>( this, "ssaoIntensity" );
		_ssaoIntensity->setMinMax( Setting::AO_INTENSITY_MIN, Setting::AO_INTENSITY_MAX );
		_ssaoBlurSize = VTX::UI::WidgetFactory::get().instantiateWidget<IntegerFieldWidget>( this, "ssaoBlurSize" );
		_ssaoBlurSize->setMinMax( Setting::AO_BLUR_SIZE_MIN, Setting::AO_BLUR_SIZE_MAX );

		_enableOutline = new QCheckBox( this );
		_outlineThickness
			= VTX::UI::WidgetFactory::get().instantiateWidget<FloatFieldWidget>( this, "outlineThickness" );
		_outlineThickness->setMinMax( Setting::OUTLINE_THICKNESS_MIN, Setting::OUTLINE_THICKNESS_MAX );
		_outlineColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "outlineColor" );

		_enableFog = new QCheckBox( this );
		_nearFog   = new QSpinBox( this );
		_nearFog->setMinimum( Setting::FOG_NEAR_MIN );
		_nearFog->setMaximum( Setting::FOG_NEAR_MAX );
		_farFog = new QSpinBox( this );
		_farFog->setMinimum( Setting::FOG_FAR_MIN );
		_farFog->setMaximum( Setting::FOG_FAR_MAX );
		_fogDensity = VTX::UI::WidgetFactory::get().instantiateWidget<FloatFieldWidget>( this, "fogDensity" );
		_fogDensity->setMinMax( 0.0f, 1.0f );
		_fogColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "fogColor" );

		_backgroundColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "backgroundColor" );
		_cameraLightColor = VTX::UI::WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "lightColor" );
		_cameraFOV		  = VTX::UI::WidgetFactory::get().instantiateWidget<FloatFieldWidget>( this, "cameraFov" );
		_cameraFOV->setMinMax( Setting::CAMERA_FOV_MIN, Setting::CAMERA_FOV_MAX );
		_cameraNear = new QSpinBox( this );
		_cameraNear->setMinimum( Setting::CAMERA_NEAR_MIN );
		_cameraNear->setMaximum( Setting::CAMERA_NEAR_MAX );
		_cameraFar = new QSpinBox( this );
		_cameraFar->setMinimum( Setting::CAMERA_FAR_MIN );
		_cameraFar->setMaximum( Setting::CAMERA_FAR_MAX );
		_antialiasing = new QCheckBox( this );
		_perspective  = new QCheckBox( this );

		QHBoxLayout * const hboxLayout = new QHBoxLayout( this );
		QVBoxLayout * const vboxLayout = new QVBoxLayout();

		_layout = new QGridLayout();
		_layout->setMargin( 0 );
		_layout->setColumnStretch( 0, 1 );
		_layout->setColumnStretch( 1, 10 );

		_addItem( _shading, QString( "Shading" ) );
		_addSpace();
		_addItem( _enableSSAO, QString( "SSAO" ) );
		_addItem( _ssaoIntensity, QString( "Intensity" ) );
		_addItem( _ssaoBlurSize, QString( "Blur Size" ) );
		_addSpace();
		_addItem( _enableOutline, QString( "Outline" ) );
		_addItem( _outlineThickness, QString( "Thickness" ) );
		_addItem( _outlineColor, QString( "Color" ) );
		_addSpace();
		_addItem( _enableFog, QString( "Fog" ) );
		_addItem( _nearFog, QString( "Near" ) );
		_addItem( _farFog, QString( "Far" ) );
		_addItem( _fogDensity, QString( "Density" ) );
		_addItem( _fogColor, QString( "Color" ) );
		_addSpace();
		_addItem( _backgroundColor, QString( "Background color" ) );
		_addItem( _cameraLightColor, QString( "Light Color" ) );
		_addItem( _cameraFOV, QString( "FOV" ) );
		_addItem( _cameraNear, QString( "Near clip" ) );
		_addItem( _cameraFar, QString( "Far clip" ) );
		_addItem( _antialiasing, QString( "Antialiasing" ) );
		_addItem( _perspective, QString( "Perspective projection" ) );

		vboxLayout->addItem( _layout );
		vboxLayout->addStretch( 1000 );
		hboxLayout->addItem( vboxLayout );
		hboxLayout->addStretch( 1000 );
	}
	void RenderEffectPresetEditor::_setupSlots()
	{
		connect( _shading,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RenderEffectPresetEditor::_onShadingChange );

		connect( _enableSSAO,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOStateChanged );
		connect( _ssaoIntensity,
				 QOverload<int>::of( &IntegerFieldWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOIntensityChanged );
		connect( _ssaoBlurSize,
				 QOverload<int>::of( &IntegerFieldWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onSSAOBlurSizeChanged );

		connect( _enableOutline,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineStateChanged );
		connect( _outlineThickness,
				 QOverload<float>::of( &FloatFieldWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineThicknessChanged );
		connect( _outlineColor,
				 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onOutlineColorChanged );

		connect( _enableFog,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onFogStateChanged );
		connect( _nearFog,
				 QOverload<int>::of( &QSpinBox::valueChanged ),
				 this,
				 &RenderEffectPresetEditor::_onFogNearChanged );
		connect(
			_farFog, QOverload<int>::of( &QSpinBox::valueChanged ), this, &RenderEffectPresetEditor::_onFogFarChanged );
		connect( _fogDensity,
				 QOverload<float>::of( &FloatFieldWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onFogDensityChanged );
		connect( _fogColor,
				 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onFogColorChanged );

		connect( _backgroundColor,
				 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onBackgroundColorChanged );
		connect( _cameraLightColor,
				 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onCameraLightColorChanged );
		connect( _cameraFOV,
				 QOverload<float>::of( &FloatFieldWidget::onValueChange ),
				 this,
				 &RenderEffectPresetEditor::_onCameraFOVChanged );

		connect( _cameraNear,
				 QOverload<int>::of( &QSpinBox::valueChanged ),
				 this,
				 &RenderEffectPresetEditor::_onCameraNearChanged );
		connect( _cameraFar,
				 QOverload<int>::of( &QSpinBox::valueChanged ),
				 this,
				 &RenderEffectPresetEditor::_onCameraFarChanged );
		connect( _antialiasing,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onAntialiasingChanged );
		connect( _perspective,
				 QOverload<int>::of( &QCheckBox::stateChanged ),
				 this,
				 &RenderEffectPresetEditor::_onPerspectiveChanged );
	}

	void RenderEffectPresetEditor::localize() {}

	void RenderEffectPresetEditor::refresh()
	{
		_shading->setCurrentIndex( (int)_preset->getShading() );

		const bool ssaoEnabled = _preset->isSSAOEnabled();
		_enableSSAO->setChecked( ssaoEnabled );
		_ssaoIntensity->setValue( _preset->getSSAOIntensity() );
		_ssaoIntensity->setEnabled( ssaoEnabled );
		_ssaoBlurSize->setValue( _preset->getSSAOBlurSize() );
		_ssaoBlurSize->setEnabled( ssaoEnabled );

		const bool outlineEnabled = _preset->isOutlineEnabled();
		_enableOutline->setChecked( outlineEnabled );
		_outlineThickness->setValue( _preset->getOutlineThickness() );
		_outlineThickness->setEnabled( outlineEnabled );
		_outlineColor->setColor( _preset->getOutlineColor() );
		_outlineColor->setEnabled( outlineEnabled );

		const bool fogEnabled = _preset->isFogEnabled();
		_enableFog->setChecked( fogEnabled );
		_nearFog->setValue( _preset->getFogNear() );
		_nearFog->setEnabled( fogEnabled );
		_farFog->setValue( _preset->getFogFar() );
		_farFog->setEnabled( fogEnabled );
		_fogDensity->setValue( _preset->getFogDensity() );
		_fogDensity->setEnabled( fogEnabled );
		_fogColor->setColor( _preset->getFogColor() );
		_fogColor->setEnabled( fogEnabled );

		_backgroundColor->setColor( _preset->getBackgroundColor() );
		_cameraLightColor->setColor( _preset->getCameraLightColor() );
		_cameraFOV->setValue( _preset->getCameraFOV() );
		_cameraNear->setValue( _preset->getCameraNearClip() );
		_cameraFar->setValue( _preset->getCameraFarClip() );
		_antialiasing->setChecked( _preset->getAA() );
		_perspective->setChecked( _preset->isPerspectiveProjection() );
	}

	void RenderEffectPresetEditor::setPreset( Model::Renderer::RenderEffectPreset * const p_model,
											  const bool								  p_updateRender )
	{
		if ( _preset == p_model )
			return;

		if ( _currentPresetView != nullptr )
		{
			MVC::MvcManager::get().deleteView( _preset, ID::View::UI_RENDER_EFFECT_PRESET );
		}

		_preset = p_model;

		_currentPresetView = MVC::MvcManager::get().instantiateView<View::UI::Widget::Renderer::RenderEffectPresetView>(
			_preset, ID::View::UI_RENDER_EFFECT_PRESET );

		_currentPresetView->setEditor( this );

		_updateRender = p_updateRender;
		refresh();
		_updateRender = true;
	}

	void RenderEffectPresetEditor::_onShadingChange( const int p_newIndex )
	{
		const VTX::Renderer::SHADING shading = VTX::Renderer::SHADING( p_newIndex );
		if ( shading != _preset->getShading() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeShading( *_preset, shading ) );
	}

	void RenderEffectPresetEditor::_onSSAOStateChanged( const int p_state )
	{
		const bool enable = p_state != 0;
		if ( enable != _preset->isSSAOEnabled() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::EnableSSAO( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onSSAOIntensityChanged( const int p_value )
	{
		if ( p_value != _preset->getSSAOIntensity() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeSSAOIntensity( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onSSAOBlurSizeChanged( const int p_value )
	{
		if ( p_value != _preset->getSSAOBlurSize() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeSSAOBlurSize( *_preset, p_value ) );
	}

	void RenderEffectPresetEditor::_onOutlineStateChanged( const int p_state )
	{
		const bool enable = p_state != 0;
		if ( enable != _preset->isOutlineEnabled() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::EnableOutline( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onOutlineThicknessChanged( const float p_value )
	{
		if ( p_value != _preset->getOutlineThickness() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeOutlineThickness( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onOutlineColorChanged( const Color::Rgb & p_color )
	{
		if ( p_color != _preset->getOutlineColor() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeOutlineColor( *_preset, p_color ) );
	}

	void RenderEffectPresetEditor::_onFogStateChanged( const int p_state )
	{
		const bool enable = p_state != 0;
		if ( enable != _preset->isFogEnabled() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::EnableFog( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onFogNearChanged()
	{
		const int value = _nearFog->value();
		if ( value != _preset->getFogNear() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeFogNear( *_preset, value ) );
	}
	void RenderEffectPresetEditor::_onFogFarChanged()
	{
		const int value = _farFog->value();
		if ( value != _preset->getFogFar() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeFogFar( *_preset, value ) );
	}
	void RenderEffectPresetEditor::_onFogDensityChanged( const float p_value )
	{
		if ( p_value != _preset->getFogDensity() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeFogDensity( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onFogColorChanged( const Color::Rgb & p_color )
	{
		if ( p_color != _preset->getFogColor() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeFogColor( *_preset, p_color ) );
	}

	void RenderEffectPresetEditor::_onBackgroundColorChanged( const Color::Rgb & p_color )
	{
		if ( p_color != _preset->getBackgroundColor() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeBackgroundColor( *_preset, p_color ) );
	}
	void RenderEffectPresetEditor::_onCameraLightColorChanged( const Color::Rgb & p_color )
	{
		if ( p_color != _preset->getCameraLightColor() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeCameraLightColor( *_preset, p_color ) );
	}
	void RenderEffectPresetEditor::_onCameraFOVChanged( const float p_value )
	{
		if ( p_value != _preset->getCameraFOV() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeCameraFOV( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onCameraNearChanged( const float p_value )
	{
		if ( p_value != _preset->getCameraNearClip() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeCameraNear( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onCameraFarChanged( const float p_value )
	{
		if ( p_value != _preset->getCameraFarClip() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeCameraFar( *_preset, p_value ) );
	}
	void RenderEffectPresetEditor::_onAntialiasingChanged( const int p_state )
	{
		const bool enable = p_state != 0;
		if ( enable != _preset->isFogEnabled() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::EnableCameraAntialiasing( *_preset, enable ) );
	}
	void RenderEffectPresetEditor::_onPerspectiveChanged( const int p_state )
	{
		const bool enable = p_state != 0;
		if ( enable != _preset->isFogEnabled() )
			VTX_ACTION_ENQUEUE( new Action::Renderer::ChangeCameraPerspective( *_preset, enable ) );
	}

	void RenderEffectPresetEditor::_applyPreset() const
	{
		if ( _updateRender )
		{
			VTX_ACTION_ENQUEUE( new Action::Renderer::ApplyRenderEffectPreset( *_preset ) );
		}
	}

	void RenderEffectPresetEditor::_addItem( QWidget * const p_widget, const QString & p_label )
	{
		QLabel * const label = new QLabel( this );
		label->setText( p_label );

		_layout->addWidget( label, _itemCount, 0 );
		_layout->addWidget( p_widget, _itemCount, 1 );

		_itemCount++;
	}
	void RenderEffectPresetEditor::_addSpace()
	{
		_layout->addItem( new QSpacerItem( 0, 10 ), _itemCount, 0, 2, 1 );
		_itemCount++;
	}
} // namespace VTX::UI::Widget::Settings
