#include "ui/qt/widget/library/graphics_config/lighting_section.hpp"
#include "ui/qt/helper.hpp"
#include <QSignalBlocker>
#include <string_view>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	namespace
	{
		float _exposureMultiplierFromEv( const float p_value ) { return Util::Math::exp2( p_value ); }

		float _exposureEvFromMultiplier( const float p_value )
		{
			const float minimumExposure = Util::Math::exp2( VTX::Renderer::TONE_MAPPING_EXPOSURE_EV_MIN );
			return Util::Math::log2( Util::Math::max( p_value, minimumExposure ) );
		}
	} // namespace

	LightingSection::LightingSection( QWidget * const p_parent ) : Section( "Lighting", p_parent )
	{
		using namespace Renderer;
		setChecked( true );
		freeze( true );

		_comboBoxShadingMode = Helper::makeQObject<QComboBox>( this );
		addWidget( _comboBoxShadingMode );
		constexpr std::array SHADING_STR { QUtf8StringView { "Diffuse" },
										   QUtf8StringView { "Glossy" },
										   QUtf8StringView { "Toon" },
										   QUtf8StringView { "Flat" },
										   QUtf8StringView { "PBR" } };

		for ( const QUtf8StringView shading : SHADING_STR )
		{
			_comboBoxShadingMode->addItem( shading.toString() );
		}

		_colorPickerLight = Helper::makeQObject<ColorPicker>( this );
		addWidget( _colorPickerLight );
		_colorPickerLight->setText( "Light" );

		_labelLightIntensity = Helper::makeQObject<QLabel>( "Light intensity", this );
		addWidget( _labelLightIntensity );
		_sliderLightIntensity = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderLightIntensity );
		_sliderLightIntensity->setMinimum( LIGHT_INTENSITY_MIN );
		_sliderLightIntensity->setMaximum( LIGHT_INTENSITY_MAX );

		_labelAmbientIntensity = Helper::makeQObject<QLabel>( "Ambient light intensity", this );
		addWidget( _labelAmbientIntensity );
		_sliderAmbientIntensity = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderAmbientIntensity );
		_sliderAmbientIntensity->setMinimum( AMBIENT_INTENSITY_MIN );
		_sliderAmbientIntensity->setMaximum( AMBIENT_INTENSITY_MAX );

		_labelToneMappingMode = Helper::makeQObject<QLabel>( "Tone mapping", this );
		addWidget( _labelToneMappingMode );
		_comboBoxToneMappingMode = Helper::makeQObject<QComboBox>( this );
		addWidget( _comboBoxToneMappingMode );
		constexpr std::array TONE_MAPPING_STR { QUtf8StringView { "None" },
												QUtf8StringView { "Classic (LDR)" },
												QUtf8StringView { "Soft (Reinhard)" },
												QUtf8StringView { "Filmic (ACES)" },
												QUtf8StringView { "Natural (Uncharted 2)" } };

		static_assert( TONE_MAPPING_STR.size() == static_cast<size_t>( E_TONE_MAPPING::COUNT ) );

		for ( const QUtf8StringView toneMapping : TONE_MAPPING_STR )
		{
			_comboBoxToneMappingMode->addItem( toneMapping.toString() );
		}

		_labelToneMappingExposure = Helper::makeQObject<QLabel>( "Exposure", this );
		addWidget( _labelToneMappingExposure );
		_sliderToneMappingExposure = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderToneMappingExposure );
		_sliderToneMappingExposure->setMinimum( TONE_MAPPING_EXPOSURE_EV_MIN );
		_sliderToneMappingExposure->setMaximum( TONE_MAPPING_EXPOSURE_EV_MAX );
		_sliderToneMappingExposure->setStep( 0.1f );
		_sliderToneMappingExposure->setDecimals( 2 );
		_sliderToneMappingExposure->setSuffix( "EV" );

		_labelSpecularFactor = Helper::makeQObject<QLabel>( "Specular factor", this );
		addWidget( _labelSpecularFactor );
		_sliderSpecularFactor = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderSpecularFactor );
		_sliderSpecularFactor->setMinimum( SPECULAR_FACTOR_MIN );
		_sliderSpecularFactor->setMaximum( SPECULAR_FACTOR_MAX );

		_labelShininess = Helper::makeQObject<QLabel>( "Shininess", this );
		addWidget( _labelShininess );
		_sliderShininess = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderShininess );
		_sliderShininess->setMinimum( SHININESS_MIN );
		_sliderShininess->setMaximum( SHININESS_MAX );

		_labelToonSteps = Helper::makeQObject<QLabel>( "Toon steps", this );
		addWidget( _labelToonSteps );
		_sliderToonSteps = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		addWidget( _sliderToonSteps );
		_sliderToonSteps->setMinimum( TOON_STEPS_MIN );
		_sliderToonSteps->setMaximum( TOON_STEPS_MAX );
		_sliderToonSteps->setStep( 1 );
		_sliderToonSteps->setDecimals( 0 );

		_materialSection = Helper::makeQObject<MaterialSection>( this );
		addWidget( _materialSection );

		connect(
			_comboBoxShadingMode,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SHADING_MODE, E_SHADING>( E_SHADING( p_index ) ); }
		);
		_colorPickerLight->onColorChanged += [ this ]( const QColor & p_color )
		{ _changeValue<E_GRAPHICS_CONFIG_VALUES::COLOR_LIGHT, Util::Color::Rgba>( Helper::fromQColor( p_color ) ); };
		connect(
			_sliderLightIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::LIGHT_INTENSITY, float>( p_value ); }
		);
		connect(
			_sliderAmbientIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::AMBIENT_INTENSITY, float>( p_value ); }
		);
		connect(
			_comboBoxToneMappingMode,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::TONE_MAPPING_MODE, E_TONE_MAPPING>( E_TONE_MAPPING( p_index ) ); }
		);
		connect(
			_sliderToneMappingExposure,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{
				_changeValue<E_GRAPHICS_CONFIG_VALUES::TONE_MAPPING_EXPOSURE, float>(
					_exposureMultiplierFromEv( p_value )
				);
			}
		);
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
	}

	void LightingSection::setConfig( const Entity p_preset, const VTX::Renderer::GraphicsConfig & p_config )
	{
		_setCurrentPreset( p_preset );
		_materialSection->setConfig( p_preset, p_config );
		const QSignalBlocker blockerMode( _comboBoxShadingMode );
		const QSignalBlocker blockerLight( _colorPickerLight );
		const QSignalBlocker blockerLightIntensity( _sliderLightIntensity );
		const QSignalBlocker blockerAmbientIntensity( _sliderAmbientIntensity );
		const QSignalBlocker blockerToneMappingMode( _comboBoxToneMappingMode );
		const QSignalBlocker blockerToneMappingExposure( _sliderToneMappingExposure );
		const QSignalBlocker blockerSpecular( _sliderSpecularFactor );
		const QSignalBlocker blockerShininess( _sliderShininess );
		const QSignalBlocker blockerToon( _sliderToonSteps );

		_comboBoxShadingMode->setCurrentIndex( static_cast<int>( p_config.shading.mode ) );
		_colorPickerLight->setColor( Helper::toQColor( p_config.shading.colorLight ) );
		_sliderLightIntensity->setValue( p_config.shading.lightIntensity );
		_sliderAmbientIntensity->setValue( p_config.shading.ambientIntensity );
		_comboBoxToneMappingMode->setCurrentIndex( static_cast<int>( p_config.toneMapping.mode ) );
		_sliderToneMappingExposure->setValue( _exposureEvFromMultiplier( p_config.toneMapping.exposure ) );
		_sliderSpecularFactor->setValue( p_config.shading.specularFactor );
		_sliderShininess->setValue( p_config.shading.shininess );
		_sliderToonSteps->setValue( p_config.shading.toonSteps );
		using namespace Renderer;
		const bool pbr			  = p_config.shading.mode == E_SHADING::PBR;
		const bool hasEnvironment = p_config.shading.backgroundMode == E_BACKGROUND_MODE::ENVIRONMENT
									&& p_config.shading.environmentPath.has_value();
		_labelLightIntensity->setVisible( pbr );
		_sliderLightIntensity->setVisible( pbr );
		_labelAmbientIntensity->setVisible( pbr && not hasEnvironment );
		_sliderAmbientIntensity->setVisible( pbr && not hasEnvironment );
		_labelToneMappingMode->setVisible( pbr );
		_comboBoxToneMappingMode->setVisible( pbr );
		_labelToneMappingExposure->setVisible( pbr );
		_sliderToneMappingExposure->setVisible( pbr );
		const bool glossy = p_config.shading.mode == E_SHADING::GLOSSY;
		const bool toon	  = p_config.shading.mode == E_SHADING::TOON;
		_labelSpecularFactor->setVisible( glossy );
		_sliderSpecularFactor->setVisible( glossy );
		_labelShininess->setVisible( glossy );
		_sliderShininess->setVisible( glossy );
		_labelToonSteps->setVisible( toon );
		_sliderToonSteps->setVisible( toon );
	}
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig
