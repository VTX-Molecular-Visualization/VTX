#include "ui/qt/widget/library/graphics_config.hpp"
#include "ui/qt/helper.hpp"
#include <app/action/graphics_config.hpp>

namespace VTX::UI::QT::Widget::Library
{
	namespace
	{
		constexpr float CHROMAB_UI_MIN = 0.f;
		constexpr float CHROMAB_UI_MAX = 5.f;

		float _chromaticAberrationToUi( const float p_value ) { return -p_value * 100.f; }

		float _chromaticAberrationFromUi( const float p_value ) { return -p_value / 100.f; }
	} // namespace

	GraphicsConfig::GraphicsConfig( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace Renderer;

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

		// Selection.
		_groupboxSelection = new HideableGroupBox( "Selection", presetGroupBox() );
		addWidget( _groupboxSelection );

		_colorPickerSelection = new ColorPicker( _groupboxSelection );
		_groupboxSelection->addWidget( _colorPickerSelection );

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

		// Chromatic aberration.
		_groupboxChromaticAberration = new HideableGroupBox( "Chromatic aberration", presetGroupBox() );
		addWidget( _groupboxChromaticAberration );

		_sliderChromaticAberrationRed = new EditableSlider( Qt::Orientation::Horizontal, _groupboxChromaticAberration );
		_groupboxChromaticAberration->addWidget( new QLabel( "Red", _groupboxChromaticAberration ) );
		_groupboxChromaticAberration->addWidget( _sliderChromaticAberrationRed );
		_sliderChromaticAberrationRed->setMinimum( CHROMAB_UI_MIN );
		_sliderChromaticAberrationRed->setMaximum( CHROMAB_UI_MAX );

		_sliderChromaticAberrationGreen
			= new EditableSlider( Qt::Orientation::Horizontal, _groupboxChromaticAberration );
		_groupboxChromaticAberration->addWidget( new QLabel( "Green", _groupboxChromaticAberration ) );
		_groupboxChromaticAberration->addWidget( _sliderChromaticAberrationGreen );
		_sliderChromaticAberrationGreen->setMinimum( CHROMAB_UI_MIN );
		_sliderChromaticAberrationGreen->setMaximum( CHROMAB_UI_MAX );

		_sliderChromaticAberrationBlue
			= new EditableSlider( Qt::Orientation::Horizontal, _groupboxChromaticAberration );
		_groupboxChromaticAberration->addWidget( new QLabel( "Blue", _groupboxChromaticAberration ) );
		_groupboxChromaticAberration->addWidget( _sliderChromaticAberrationBlue );
		_sliderChromaticAberrationBlue->setMinimum( CHROMAB_UI_MIN );
		_sliderChromaticAberrationBlue->setMaximum( CHROMAB_UI_MAX );

		// Pixelize.
		_groupboxPixelize = new HideableGroupBox( "Pixelize", presetGroupBox() );
		addWidget( _groupboxPixelize );

		_sliderPixelizeSize = new EditableSlider( Qt::Orientation::Horizontal, _groupboxPixelize );
		_groupboxPixelize->addWidget( new QLabel( "Size", _groupboxPixelize ) );
		_groupboxPixelize->addWidget( _sliderPixelizeSize );
		_sliderPixelizeSize->setMinimum( PIXELIZE_SIZE_MIN );
		_sliderPixelizeSize->setMaximum( PIXELIZE_SIZE_MAX );
		_sliderPixelizeSize->setStep( 1 );
		_sliderPixelizeSize->setDecimals( 0 );

		_checkBoxPixelizeBackground = new QCheckBox( "Background", _groupboxPixelize );
		_groupboxPixelize->addWidget( _checkBoxPixelizeBackground );

		// Connect widget callbacks.
		using namespace Renderer;
		connect(
			_comboBoxShadingMode,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SHADING_MODE, E_SHADING>( E_SHADING( p_index ) ); }
		);

		_colorPickerBackground->onColorChanged += [ this ]( const QColor & p_color )
		{
			_changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_BACKGROUND, Util::Color::Rgba>(
				Helper::fromQColor( p_color )
			);
		};

		_colorPickerLight->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_LIGHT, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderSpecularFactor,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SPECULAR_FACTOR, float>( p_value ); }
		);

		connect(
			_sliderShininess,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::SHININESS, float>( p_value ); }
		);

		connect(
			_sliderToonSteps,
			&EditableSlider::valueChanged,
			[ this ]( const uint p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::TOON_STEPS, uint>( p_value ); }
		);

		connect(
			_groupboxSSAO,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO, bool>( p_state ); }
		);

		connect(
			_sliderSSAOIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY, float>( p_value ); }
		);

		connect(
			_sliderBlurSize,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE, float>( p_value ); }
		);

		connect(
			_groupboxOutline,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_OUTLINE, bool>( p_state ); }
		);

		_colorPickerOutline->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_OUTLINE, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderOutlineSensitivity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::OUTLINE_SENSITIVITY, float>( p_value ); }
		);

		connect(
			_sliderOutlineThickness,
			&EditableSlider::valueChanged,
			[ this ]( const uint p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::OUTLINE_THICKNESS, uint>( p_value ); }
		);

		connect(
			_groupboxFog,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_FOG, bool>( p_state ); }
		);

		_colorPickerFog->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_FOG, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };

		connect(
			_sliderFogNear,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_NEAR, float>( p_value ); }
		);

		connect(
			_sliderFogFar,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_FAR, float>( p_value ); }
		);

		connect(
			_sliderFogDensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::FOG_DENSITY, float>( p_value ); }
		);

		connect(
			_groupboxChromaticAberration,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_CHROMAB, bool>( p_state ); }
		);

		connect(
			_sliderChromaticAberrationRed,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_RED, float>( _chromaticAberrationFromUi( p_value ) ); }
		);

		connect(
			_sliderChromaticAberrationGreen,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_GREEN, float>( _chromaticAberrationFromUi( p_value ) ); }
		);

		connect(
			_sliderChromaticAberrationBlue,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CHROMAB_BLUE, float>( _chromaticAberrationFromUi( p_value ) ); }
		);

		connect(
			_groupboxPixelize,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_PIXELIZE, bool>( p_state ); }
		);

		connect(
			_sliderPixelizeSize,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::PIXELIZE_SIZE, uint>( static_cast<uint>( p_value ) ); }
		);

		connect(
			_checkBoxPixelizeBackground,
			&QCheckBox::toggled,
			[ this ]( const bool p_state )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::PIXELIZE_BACKGROUND, bool>( p_state ); }
		);

		connect(
			_groupboxSelection,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SELECTION, bool>( p_state ); }
		);

		_colorPickerSelection->onColorChanged += [ this ]( const QColor & p_color )
		{
			_changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_SELECTION, Util::Color::Rgba>( Helper::fromQColor( p_color ) );
		};
	}

	void GraphicsConfig::_update( Entity p_e )
	{
		auto & preset = App::REG().get<Renderer::GraphicsConfig>( p_e );

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
		const QSignalBlocker blocker18( _groupboxChromaticAberration );
		const QSignalBlocker blocker19( _sliderChromaticAberrationRed );
		const QSignalBlocker blocker20( _sliderChromaticAberrationGreen );
		const QSignalBlocker blocker21( _sliderChromaticAberrationBlue );
		const QSignalBlocker blocker22( _groupboxPixelize );
		const QSignalBlocker blocker23( _sliderPixelizeSize );
		const QSignalBlocker blocker24( _checkBoxPixelizeBackground );
		const QSignalBlocker blocker25( _groupboxSelection );
		const QSignalBlocker blocker26( _colorPickerSelection );

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
		_groupboxChromaticAberration->setChecked( preset.activeChromaticAberration );
		_sliderChromaticAberrationRed->setValue( _chromaticAberrationToUi( preset.chromaticAberrationRed ) );
		_sliderChromaticAberrationGreen->setValue( _chromaticAberrationToUi( preset.chromaticAberrationGreen ) );
		_sliderChromaticAberrationBlue->setValue( _chromaticAberrationToUi( preset.chromaticAberrationBlue ) );
		_groupboxPixelize->setChecked( preset.activePixelize );
		_sliderPixelizeSize->setValue( preset.pixelizeSize );
		_checkBoxPixelizeBackground->setChecked( preset.pixelizeBackground );
		_groupboxSelection->setChecked( preset.activeSelection );
		_colorPickerSelection->setColor( Helper::toQColor( preset.colorSelection ) );

		_applyLogic( preset );
	}

	void GraphicsConfig::_applyLogic( const Renderer::GraphicsConfig & p_preset )
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
