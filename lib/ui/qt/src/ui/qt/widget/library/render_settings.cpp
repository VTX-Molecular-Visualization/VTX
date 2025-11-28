#include "ui/qt/widget/library/render_settings.hpp"
#include "ui/qt/helper.hpp"
#include <app/action/render_settings.hpp>

namespace VTX::UI::QT::Widget::Library
{
	RenderSettings::RenderSettings( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace Renderer;
		using namespace VTX::Renderer::Settings;

		setTitle( "Edit render settings" );

		// Shading.
		_groupboxShading = new HideableGroupBox( "Lighting", presetGroupBox() );
		_groupboxShading->setChecked( true );
		_groupboxShading->freeze( true );
		addWidget( _groupboxShading );

		_comboBoxShadingMode = new QComboBox( _groupboxShading );
		_groupboxShading->addWidget( _comboBoxShadingMode );
		for ( int i = 0; i < int( E_SHADING::COUNT ); ++i )
		{
			_comboBoxShadingMode->addItem( SHADING_STR[ i ].data() );
		}

		_colorPickerBackground = new ColorPicker( _groupboxShading );
		_groupboxShading->addWidget( _colorPickerBackground );
		_colorPickerBackground->setText( "Background" );

		_colorPickerLight = new ColorPicker( _groupboxShading );
		_groupboxShading->addWidget( _colorPickerLight );
		_colorPickerLight->setText( "Light" );

		_sliderSpecularFactor = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelSpecularFactor  = new QLabel( "Specular factor", _groupboxShading );
		_groupboxShading->addWidget( _labelSpecularFactor );
		_groupboxShading->addWidget( _sliderSpecularFactor );
		_sliderSpecularFactor->setMinimum( SPECULAR_FACTOR_MIN );
		_sliderSpecularFactor->setMaximum( SPECULAR_FACTOR_MAX );

		_sliderShininess = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelShininess	 = new QLabel( "Shininess", _groupboxShading );
		_groupboxShading->addWidget( _labelShininess );
		_groupboxShading->addWidget( _sliderShininess );
		_sliderShininess->setMinimum( SHININESS_MIN );
		_sliderShininess->setMaximum( SHININESS_MAX );

		_sliderToonSteps = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelToonSteps	 = new QLabel( "Toon steps", _groupboxShading );
		_groupboxShading->addWidget( _labelToonSteps );
		_groupboxShading->addWidget( _sliderToonSteps );
		_sliderToonSteps->setMinimum( TOON_STEPS_MIN );
		_sliderToonSteps->setMaximum( TOON_STEPS_MAX );
		_sliderToonSteps->setStep( 1 );
		_sliderToonSteps->setDecimals( 0 );

		// SSAO.
		_groupboxSSAO = new HideableGroupBox( "Shadows", presetGroupBox() );
		addWidget( _groupboxSSAO );

		_sliderSSAOIntensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSSAO );
		_groupboxSSAO->addWidget( new QLabel( "Intensity", _groupboxSSAO ) );
		_groupboxSSAO->addWidget( _sliderSSAOIntensity );
		_sliderSSAOIntensity->setMinimum( SSAO_INTENSITY_MIN );
		_sliderSSAOIntensity->setMaximum( SSAO_INTENSITY_MAX );

		_sliderBlurSize = new EditableSlider( Qt::Orientation::Horizontal, _groupboxSSAO );
		_groupboxSSAO->addWidget( new QLabel( "Blur size", _groupboxSSAO ) );
		_groupboxSSAO->addWidget( _sliderBlurSize );
		_sliderBlurSize->setMinimum( BLUR_SIZE_MIN );
		_sliderBlurSize->setMaximum( BLUR_SIZE_MAX );

		// Outline.
		_groupboxOutline = new HideableGroupBox( "Outline", presetGroupBox() );
		addWidget( _groupboxOutline );

		_colorPickerOutline = new ColorPicker( _groupboxOutline );
		_groupboxOutline->addWidget( _colorPickerOutline );

		_sliderOutlineSensitivity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxOutline );
		_groupboxOutline->addWidget( new QLabel( "Sensitivity", _groupboxOutline ) );
		_groupboxOutline->addWidget( _sliderOutlineSensitivity );
		_sliderOutlineSensitivity->setMinimum( OUTLINE_SENSITIVITY_MIN );
		_sliderOutlineSensitivity->setMaximum( OUTLINE_SENSITIVITY_MAX );

		_sliderOutlineThickness = new EditableSlider( Qt::Orientation::Horizontal, _groupboxOutline );
		_groupboxOutline->addWidget( new QLabel( "Thickness", _groupboxOutline ) );
		_groupboxOutline->addWidget( _sliderOutlineThickness );
		_sliderOutlineThickness->setMinimum( OUTLINE_THICKNESS_MIN );
		_sliderOutlineThickness->setMaximum( OUTLINE_THICKNESS_MAX );
		_sliderOutlineThickness->setStep( 1 );
		_sliderOutlineThickness->setDecimals( 0 );

		// Fog.
		_groupboxFog = new HideableGroupBox( "Fog", presetGroupBox() );
		addWidget( _groupboxFog );

		_colorPickerFog = new ColorPicker( _groupboxFog );
		_groupboxFog->addWidget( _colorPickerFog );

		_sliderFogNear = new EditableSlider( Qt::Orientation::Horizontal, _groupboxFog );
		_groupboxFog->addWidget( new QLabel( "Near", _groupboxFog ) );
		_groupboxFog->addWidget( _sliderFogNear );
		_sliderFogNear->setMinimum( FOG_NEAR_MIN );
		_sliderFogNear->setMaximum( FOG_NEAR_MAX );

		_sliderFogFar = new EditableSlider( Qt::Orientation::Horizontal, _groupboxFog );
		_groupboxFog->addWidget( new QLabel( "Far", _groupboxFog ) );
		_groupboxFog->addWidget( _sliderFogFar );
		_sliderFogFar->setMinimum( FOG_FAR_MIN );
		_sliderFogFar->setMaximum( FOG_FAR_MAX );

		_sliderFogDensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxFog );
		_groupboxFog->addWidget( new QLabel( "Density", _groupboxFog ) );
		_groupboxFog->addWidget( _sliderFogDensity );
		_sliderFogDensity->setMinimum( FOG_DENSITY_MIN );
		_sliderFogDensity->setMinimum( FOG_DENSITY_MAX );

		// Selection.
		_groupboxSelection = new HideableGroupBox( "Selection", presetGroupBox() );
		addWidget( _groupboxSelection );

		_colorPickerSelection = new ColorPicker( _groupboxSelection );
		_groupboxSelection->addWidget( _colorPickerSelection );

		// Connect widget callbacks.
		connect(
			_comboBoxShadingMode,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_RENDER_SETTINGS::SHADING_MODE, E_SHADING>( E_SHADING( p_index ) ); }
		);

		_colorPickerBackground->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_RENDER_SETTINGS::COLOR_BACKGROUND, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		_colorPickerLight->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_RENDER_SETTINGS::COLOR_LIGHT, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderSpecularFactor,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::SPECULAR_FACTOR, float>( p_value ); }
		);

		connect(
			_sliderShininess,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::SHININESS, float>( p_value ); }
		);

		connect(
			_sliderToonSteps,
			&EditableSlider::valueChanged,
			[ this ]( const uint p_value ) { _changeValue<E_RENDER_SETTINGS::TOON_STEPS, uint>( p_value ); }
		);

		connect(
			_groupboxSSAO,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_RENDER_SETTINGS::ACTIVE_SSAO, bool>( p_state ); }
		);

		connect(
			_sliderSSAOIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::SSAO_INTENSITY, float>( p_value ); }
		);

		connect(
			_sliderBlurSize,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::BLUR_SIZE, float>( p_value ); }
		);

		connect(
			_groupboxOutline,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_RENDER_SETTINGS::ACTIVE_OUTLINE, bool>( p_state ); }
		);

		_colorPickerOutline->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_RENDER_SETTINGS::COLOR_OUTLINE, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderOutlineSensitivity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, float>( p_value ); }
		);

		connect(
			_sliderOutlineThickness,
			&EditableSlider::valueChanged,
			[ this ]( const uint p_value ) { _changeValue<E_RENDER_SETTINGS::OUTLINE_THICKNESS, uint>( p_value ); }
		);

		connect(
			_groupboxFog,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_RENDER_SETTINGS::ACTIVE_FOG, bool>( p_state ); }
		);

		_colorPickerFog->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_RENDER_SETTINGS::COLOR_FOG, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderFogNear,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::FOG_NEAR, float>( p_value ); }
		);

		connect(
			_sliderFogFar,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::FOG_FAR, float>( p_value ); }
		);

		connect(
			_sliderFogDensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_RENDER_SETTINGS::FOG_DENSITY, float>( p_value ); }
		);

		connect(
			_groupboxSelection,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_RENDER_SETTINGS::ACTIVE_SELECTION, bool>( p_state ); }
		);

		_colorPickerSelection->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_RENDER_SETTINGS::COLOR_SELECTION, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };
	}

	void RenderSettings::_update( App::ECS::Entity p_e )
	{
		auto & preset = App::REG().get<Renderer::RenderSettings>( p_e );

		const QSignalBlocker blocker0( _comboBoxShadingMode );
		const QSignalBlocker blocker1( _colorPickerBackground );
		const QSignalBlocker blocker2( _colorPickerLight );
		const QSignalBlocker blocker3( _sliderSpecularFactor );
		const QSignalBlocker blocker4( _sliderShininess );
		const QSignalBlocker blocker5( _sliderToonSteps );
		const QSignalBlocker blocker6( _groupboxSSAO );
		const QSignalBlocker blocker7( _sliderSSAOIntensity );
		const QSignalBlocker blocker8( _sliderBlurSize );
		const QSignalBlocker blocker9( _groupboxOutline );
		const QSignalBlocker blocker10( _colorPickerOutline );
		const QSignalBlocker blocker11( _sliderOutlineSensitivity );
		const QSignalBlocker blocker12( _sliderOutlineThickness );
		const QSignalBlocker blocker13( _groupboxFog );
		const QSignalBlocker blocker14( _colorPickerFog );
		const QSignalBlocker blocker15( _sliderFogNear );
		const QSignalBlocker blocker16( _sliderFogFar );
		const QSignalBlocker blocker17( _sliderFogDensity );
		const QSignalBlocker blocker18( _groupboxSelection );
		const QSignalBlocker blocker19( _colorPickerSelection );

		_comboBoxShadingMode->setCurrentIndex( int( preset.shadingMode ) );
		_colorPickerBackground->setColor( Helper::toQColor( preset.colorBackground ) );
		_colorPickerLight->setColor( Helper::toQColor( preset.colorLight ) );
		_sliderSpecularFactor->setValue( preset.specularFactor );
		_sliderShininess->setValue( preset.shininess );
		_sliderToonSteps->setValue( preset.toonSteps );
		_groupboxSSAO->setChecked( preset.activeSSAO );
		_sliderSSAOIntensity->setValue( preset.ssaoIntensity );
		_sliderBlurSize->setValue( preset.blurSize );
		_groupboxOutline->setChecked( preset.activeOutline );
		_colorPickerOutline->setColor( Helper::toQColor( preset.colorOutline ) );
		_sliderOutlineSensitivity->setValue( preset.outlineSensitivity );
		_sliderOutlineThickness->setValue( preset.outlineThickness );
		_groupboxFog->setChecked( preset.activeFog );
		_colorPickerFog->setColor( Helper::toQColor( preset.colorFog ) );
		_sliderFogNear->setValue( preset.fogNear );
		_sliderFogFar->setValue( preset.fogFar );
		_sliderFogDensity->setValue( preset.fogDensity );
		_groupboxSelection->setChecked( preset.activeSelection );
		_colorPickerSelection->setColor( Helper::toQColor( preset.colorSelection ) );

		_applyLogic( preset );
	}

	void RenderSettings::_applyLogic( const Renderer::RenderSettings & p_preset )
	{
		using namespace Renderer;

		switch ( p_preset.shadingMode )
		{
		case E_SHADING::DIFFUSE:
			_labelSpecularFactor->setVisible( false );
			_sliderSpecularFactor->setVisible( false );
			_labelShininess->setVisible( false );
			_sliderShininess->setVisible( false );
			_labelToonSteps->setVisible( false );
			_sliderToonSteps->setVisible( false );
			break;
		case E_SHADING::GLOSSY:
			_labelSpecularFactor->setVisible( true );
			_sliderSpecularFactor->setVisible( true );
			_labelShininess->setVisible( true );
			_sliderShininess->setVisible( true );
			_labelToonSteps->setVisible( false );
			_sliderToonSteps->setVisible( false );
			break;
		case E_SHADING::TOON:
			_labelSpecularFactor->setVisible( false );
			_sliderSpecularFactor->setVisible( false );
			_labelShininess->setVisible( false );
			_sliderShininess->setVisible( false );
			_labelToonSteps->setVisible( true );
			_sliderToonSteps->setVisible( true );
			break;
		case E_SHADING::FLAT:
			_labelSpecularFactor->setEnabled( false );
			_sliderSpecularFactor->setEnabled( false );
			_labelShininess->setEnabled( false );
			_sliderShininess->setEnabled( false );
			_labelToonSteps->setEnabled( false );
			_sliderToonSteps->setEnabled( false );
			break;
		}
	}
} // namespace VTX::UI::QT::Widget::Library
