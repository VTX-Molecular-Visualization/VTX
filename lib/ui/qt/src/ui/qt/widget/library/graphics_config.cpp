#include "ui/qt/widget/library/graphics_config.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QFileDialog>
#include <QHBoxLayout>
#include <app/action/graphics_config.hpp>
#include <app/arguments.hpp>
#include <app/generic/name.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Widget::Library
{
	namespace
	{
		constexpr float CHROMAB_UI_MIN				= 0.f;
		constexpr float CHROMAB_UI_MAX				= 5.f;
		constexpr float ENVIRONMENT_ROTATION_UI_MIN = 0.f;
		constexpr float ENVIRONMENT_ROTATION_UI_MAX = 360.f;

		float _chromaticAberrationToUi( const float p_value ) { return -p_value * 100.f; }

		float _chromaticAberrationFromUi( const float p_value ) { return -p_value / 100.f; }

		float _ssaoScaleFromUi( const int p_index )
		{
			switch ( p_index )
			{
			case 1: return 2.f;
			case 0:
			default: return 1.f;
			}
		}

		int _ssaoScaleToUi( const float p_value )
		{
			if ( p_value == 2.f )
			{
				return 1;
			}
			return 0;
		}
	} // namespace

	GraphicsConfig::GraphicsConfig( QWidget * p_parent ) : BasePresetWidget( p_parent )
	{
		using namespace Renderer;

		setTitle( "Edit render settings" );

		// Background.
		_groupboxBackground = new HideableGroupBox( "Background", presetGroupBox() );
		_groupboxBackground->setChecked( true );
		_groupboxBackground->freeze( true );
		addWidget( _groupboxBackground );

		_colorPickerBackground = new ColorPicker( _groupboxBackground );
		_groupboxBackground->addWidget( _colorPickerBackground );
		_colorPickerBackground->setText( "Color" );

		_groupboxBackground->addWidget( new QLabel( "Environment map", _groupboxBackground ) );
		auto * const environmentPathWidget = new QWidget( _groupboxBackground );
		auto * const environmentPathLayout = new QHBoxLayout( environmentPathWidget );
		environmentPathLayout->setContentsMargins( 0, 0, 0, 0 );
		_lineEnvironmentPath = new QLineEdit( environmentPathWidget );
		_lineEnvironmentPath->setReadOnly( true );
		_buttonEnvironmentBrowse = new QPushButton( environmentPathWidget );
		_buttonEnvironmentBrowse->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
		_buttonEnvironmentBrowse->setToolTip( "Open environment map" );
		_buttonEnvironmentBrowse->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
		_buttonEnvironmentBrowse->setMinimumWidth( 0 );
		_buttonEnvironmentClear = new QPushButton( environmentPathWidget );
		_buttonEnvironmentClear->setIcon( STYLE().iconFromCodepoint( Style::Icons::DELETE ) );
		_buttonEnvironmentClear->setToolTip( "Remove environment map" );
		_buttonEnvironmentClear->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
		_buttonEnvironmentClear->setMinimumWidth( 0 );
		environmentPathLayout->addWidget( _lineEnvironmentPath );
		environmentPathLayout->addWidget( _buttonEnvironmentBrowse );
		environmentPathLayout->addWidget( _buttonEnvironmentClear );
		_groupboxBackground->addWidget( environmentPathWidget );

		_labelEnvironmentRotation = new QLabel( "Environment rotation", _groupboxBackground );
		_groupboxBackground->addWidget( _labelEnvironmentRotation );
		_sliderEnvironmentRotation = new EditableSlider( Qt::Orientation::Horizontal, _groupboxBackground );
		_groupboxBackground->addWidget( _sliderEnvironmentRotation );
		_sliderEnvironmentRotation->setMinimum( ENVIRONMENT_ROTATION_UI_MIN );
		_sliderEnvironmentRotation->setMaximum( ENVIRONMENT_ROTATION_UI_MAX );
		_sliderEnvironmentRotation->setStep( 1.f );
		_sliderEnvironmentRotation->setDecimals( 1 );
		_sliderEnvironmentRotation->setSuffix( "°" );

		// Shading.
		_groupboxShading = new HideableGroupBox( "Lighting", presetGroupBox() );
		_groupboxShading->setChecked( true );
		_groupboxShading->freeze( true );
		addWidget( _groupboxShading );

		_comboBoxShadingMode = new QComboBox( _groupboxShading );
		_groupboxShading->addWidget( _comboBoxShadingMode );

		constexpr std::string_view SHADING_STR[ int( E_SHADING::COUNT ) ]
			= { "Diffuse", "Glossy", "Toon", "Flat", "PBR" };
		for ( int i = 0; i < int( E_SHADING::COUNT ); ++i )
		{
			_comboBoxShadingMode->addItem( SHADING_STR[ i ].data() );
		}

		_colorPickerLight = new ColorPicker( _groupboxShading );
		_groupboxShading->addWidget( _colorPickerLight );
		_colorPickerLight->setText( "Light" );

		_sliderLightIntensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelLightIntensity  = new QLabel( "Light intensity", _groupboxShading );
		_groupboxShading->addWidget( _labelLightIntensity );
		_groupboxShading->addWidget( _sliderLightIntensity );
		_sliderLightIntensity->setMinimum( LIGHT_INTENSITY_MIN );
		_sliderLightIntensity->setMaximum( LIGHT_INTENSITY_MAX );

		_sliderAmbientIntensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelAmbientIntensity	= new QLabel( "Ambient light intensity", _groupboxShading );
		_groupboxShading->addWidget( _labelAmbientIntensity );
		_groupboxShading->addWidget( _sliderAmbientIntensity );
		_sliderAmbientIntensity->setMinimum( AMBIENT_INTENSITY_MIN );
		_sliderAmbientIntensity->setMaximum( AMBIENT_INTENSITY_MAX );

		_labelToneMappingMode = new QLabel( "Tone mapping", _groupboxShading );
		_groupboxShading->addWidget( _labelToneMappingMode );
		_comboBoxToneMappingMode = new QComboBox( _groupboxShading );
		_groupboxShading->addWidget( _comboBoxToneMappingMode );
		constexpr std::string_view TONE_MAPPING_STR[ int( E_TONE_MAPPING::COUNT ) ]
			= { "None", "Reinhard", "ACES Film" };
		for ( int i = 0; i < int( E_TONE_MAPPING::COUNT ); ++i )
		{
			_comboBoxToneMappingMode->addItem( TONE_MAPPING_STR[ i ].data() );
		}

		_sliderToneMappingExposure = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_labelToneMappingExposure  = new QLabel( "Exposure", _groupboxShading );
		_groupboxShading->addWidget( _labelToneMappingExposure );
		_groupboxShading->addWidget( _sliderToneMappingExposure );
		_sliderToneMappingExposure->setMinimum( TONE_MAPPING_EXPOSURE_MIN );
		_sliderToneMappingExposure->setMaximum( TONE_MAPPING_EXPOSURE_MAX );

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

		_labelMaterial = new QLabel( "Material", _groupboxShading );
		_groupboxShading->addWidget( _labelMaterial );
		_listMaterials = new QListWidget( _groupboxShading );
		_listMaterials->setSelectionMode( QAbstractItemView::SingleSelection );
		_listMaterials->setSortingEnabled( true );
		_groupboxShading->addWidget( _listMaterials );

		auto materialPresets = App::REG().view<App::Generic::Name, Material>();
		for ( const Entity entity : materialPresets )
		{
			auto * const item = new QListWidgetItem(
				QString::fromStdString( materialPresets.get<App::Generic::Name>( entity ).name ), _listMaterials
			);
			item->setData( Qt::UserRole, QVariant::fromValue<Entity>( entity ) );
		}

		_labelMaterialMetallic = new QLabel( "Metallic", _groupboxShading );
		_groupboxShading->addWidget( _labelMaterialMetallic );
		_sliderMaterialMetallic = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_groupboxShading->addWidget( _sliderMaterialMetallic );
		_sliderMaterialMetallic->setMinimum( 0.f );
		_sliderMaterialMetallic->setMaximum( 1.f );

		_labelMaterialRoughness = new QLabel( "Roughness", _groupboxShading );
		_groupboxShading->addWidget( _labelMaterialRoughness );
		_sliderMaterialRoughness = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_groupboxShading->addWidget( _sliderMaterialRoughness );
		_sliderMaterialRoughness->setMinimum( 0.f );
		_sliderMaterialRoughness->setMaximum( 1.f );

		_labelMaterialEmissiveIntensity = new QLabel( "Emissive intensity", _groupboxShading );
		_groupboxShading->addWidget( _labelMaterialEmissiveIntensity );
		_sliderMaterialEmissiveIntensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxShading );
		_groupboxShading->addWidget( _sliderMaterialEmissiveIntensity );
		_sliderMaterialEmissiveIntensity->setMinimum( MATERIAL_EMISSIVE_INTENSITY_MIN );
		_sliderMaterialEmissiveIntensity->setMaximum( MATERIAL_EMISSIVE_INTENSITY_MAX );

		// SSAO.
		_groupboxSSAO = new HideableGroupBox( "Shadows", presetGroupBox() );
		addWidget( _groupboxSSAO );

		_comboBoxSSAOMethod = new QComboBox( _groupboxSSAO );
		_groupboxSSAO->addWidget( _comboBoxSSAOMethod );

		constexpr std::string_view SSAO_METHOD_STR[ int( E_SSAO_METHOD::COUNT ) ]
			= { "SSAO", "SSAO line", "SAO", "BMGTAO", "HBAO" };
		for ( int i = 0; i < int( E_SSAO_METHOD::COUNT ); ++i )
		{
			_comboBoxSSAOMethod->addItem( SSAO_METHOD_STR[ i ].data() );
		}

		_comboBoxSSAOScale = new QComboBox( _groupboxSSAO );
		_groupboxSSAO->addWidget( _comboBoxSSAOScale );
		_comboBoxSSAOScale->addItem( "Normal" );
		_comboBoxSSAOScale->addItem( "Downscale" );

		const bool showDebugSSAOSettings = App::ARGS().debug;
		_comboBoxSSAOMethod->setVisible( showDebugSSAOSettings );
		_comboBoxSSAOScale->setVisible( showDebugSSAOSettings );

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

		// CRT.
		_groupboxCRT = new HideableGroupBox( "CRT", presetGroupBox() );
		addWidget( _groupboxCRT );

		_sliderCRTCurvatureX = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Curvature X", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTCurvatureX );
		_sliderCRTCurvatureX->setMinimum( CRT_CURVATURE_MIN );
		_sliderCRTCurvatureX->setMaximum( CRT_CURVATURE_MAX );

		_sliderCRTCurvatureY = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Curvature Y", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTCurvatureY );
		_sliderCRTCurvatureY->setMinimum( CRT_CURVATURE_MIN );
		_sliderCRTCurvatureY->setMaximum( CRT_CURVATURE_MAX );

		_sliderCRTRatio = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Ratio", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTRatio );
		_sliderCRTRatio->setMinimum( CRT_RATIO_MIN );
		_sliderCRTRatio->setMaximum( CRT_RATIO_MAX );

		_sliderCRTGraninessX = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Graniness X", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTGraninessX );
		_sliderCRTGraninessX->setMinimum( CRT_GRANINESS_MIN );
		_sliderCRTGraninessX->setMaximum( CRT_GRANINESS_MAX );

		_sliderCRTGraninessY = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Graniness Y", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTGraninessY );
		_sliderCRTGraninessY->setMinimum( CRT_GRANINESS_MIN );
		_sliderCRTGraninessY->setMaximum( CRT_GRANINESS_MAX );

		_sliderCRTVignetteRoundness = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Vignette roundness", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTVignetteRoundness );
		_sliderCRTVignetteRoundness->setMinimum( CRT_VIGNETTE_ROUNDNESS_MIN );
		_sliderCRTVignetteRoundness->setMaximum( CRT_VIGNETTE_ROUNDNESS_MAX );

		_sliderCRTVignetteIntensity = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Vignette intensity", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTVignetteIntensity );
		_sliderCRTVignetteIntensity->setMinimum( CRT_VIGNETTE_INTENSITY_MIN );
		_sliderCRTVignetteIntensity->setMaximum( CRT_VIGNETTE_INTENSITY_MAX );

		_sliderCRTBrightness = new EditableSlider( Qt::Orientation::Horizontal, _groupboxCRT );
		_groupboxCRT->addWidget( new QLabel( "Brightness", _groupboxCRT ) );
		_groupboxCRT->addWidget( _sliderCRTBrightness );
		_sliderCRTBrightness->setMinimum( CRT_BRIGHTNESS_MIN );
		_sliderCRTBrightness->setMaximum( CRT_BRIGHTNESS_MAX );

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
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::TONE_MAPPING_EXPOSURE, float>( p_value ); }
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

		connect(
			_listMaterials,
			&QListWidget::itemClicked,
			[ this ]( QListWidgetItem * const p_item )
			{
				const Entity materialPreset = p_item->data( Qt::UserRole ).value<Entity>();
				_changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL, Material>(
					App::REG().get<Material>( materialPreset )
				);
			}
		);

		connect(
			_sliderMaterialMetallic,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_METALLIC, float>( p_value ); }
		);

		connect(
			_sliderMaterialRoughness,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_ROUGHNESS, float>( p_value ); }
		);

		connect(
			_sliderMaterialEmissiveIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::MATERIAL_EMISSIVE_INTENSITY, float>( p_value ); }
		);

		connect(
			_buttonEnvironmentBrowse,
			&QPushButton::clicked,
			[ this ]
			{
				const auto &  config = App::REG().get<VTX::Renderer::GraphicsConfig>( currentPreset() );
				const QString initialPath
					= config.shading.environmentPath
						  ? QString::fromStdString( config.shading.environmentPath->parent_path().string() )
						  : QString {};
				const QString selectedPath = QFileDialog::getOpenFileName(
					this, "Select environment map", initialPath, "HDR environment maps (*.exr *.EXR *.hdr *.HDR)"
				);
				if ( not selectedPath.isEmpty() )
				{
					_changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>(
						FilePath( selectedPath.toStdString() )
					);
				}
			}
		);

		connect(
			_buttonEnvironmentClear,
			&QPushButton::clicked,
			[ this ] { _changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>( FilePath {} ); }
		);

		connect(
			_sliderEnvironmentRotation,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_ROTATION, float>( Util::Math::radians( p_value ) ); }
		);

		connect(
			_groupboxSSAO,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO, bool>( p_state ); }
		);

		connect(
			_comboBoxSSAOMethod,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_METHOD, E_SSAO_METHOD>( E_SSAO_METHOD( p_index ) ); }
		);

		connect(
			_comboBoxSSAOScale,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			[ this ]( const int p_index )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::SSAO_SCALE, float>( _ssaoScaleFromUi( p_index ) ); }
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
			_groupboxCRT,
			&HideableGroupBox::toggled,
			[ this ]( const bool p_state ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::ACTIVE_CRT, bool>( p_state ); }
		);

		connect(
			_sliderCRTCurvatureX,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_X, float>( p_value ); }
		);

		connect(
			_sliderCRTCurvatureY,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_Y, float>( p_value ); }
		);

		connect(
			_sliderCRTRatio,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_RATIO, float>( p_value ); }
		);

		connect(
			_sliderCRTGraninessX,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_X, float>( p_value ); }
		);

		connect(
			_sliderCRTGraninessY,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_Y, float>( p_value ); }
		);

		connect(
			_sliderCRTVignetteRoundness,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_ROUNDNESS, float>( p_value ); }
		);

		connect(
			_sliderCRTVignetteIntensity,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_INTENSITY, float>( p_value ); }
		);

		connect(
			_sliderCRTBrightness,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value )
			{ _changeValue<E_GRAPHICS_CONFIG_VALUES::CRT_BRIGHTNESS, float>( p_value ); }
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
		const QSignalBlocker blockerLightIntensity( _sliderLightIntensity );
		const QSignalBlocker blockerAmbientIntensity( _sliderAmbientIntensity );
		const QSignalBlocker blockerToneMappingMode( _comboBoxToneMappingMode );
		const QSignalBlocker blockerToneMappingExposure( _sliderToneMappingExposure );
		const QSignalBlocker blocker3( _sliderSpecularFactor );
		const QSignalBlocker blocker4( _sliderShininess );
		const QSignalBlocker blocker5( _sliderToonSteps );
		const QSignalBlocker blockerMaterial( _listMaterials );
		const QSignalBlocker blockerMaterialMetallic( _sliderMaterialMetallic );
		const QSignalBlocker blockerMaterialRoughness( _sliderMaterialRoughness );
		const QSignalBlocker blockerMaterialEmissiveIntensity( _sliderMaterialEmissiveIntensity );
		const QSignalBlocker blockerEnvironmentRotation( _sliderEnvironmentRotation );
		const QSignalBlocker blocker6( _groupboxSSAO );
		const QSignalBlocker blocker7( _comboBoxSSAOMethod );
		const QSignalBlocker blocker8( _comboBoxSSAOScale );
		const QSignalBlocker blocker9( _sliderSSAOIntensity );
		const QSignalBlocker blocker10( _sliderBlurSize );
		const QSignalBlocker blocker11( _groupboxOutline );
		const QSignalBlocker blocker12( _colorPickerOutline );
		const QSignalBlocker blocker13( _sliderOutlineSensitivity );
		const QSignalBlocker blocker14( _sliderOutlineThickness );
		const QSignalBlocker blocker15( _groupboxFog );
		const QSignalBlocker blocker16( _colorPickerFog );
		const QSignalBlocker blocker17( _sliderFogNear );
		const QSignalBlocker blocker18( _sliderFogFar );
		const QSignalBlocker blocker36( _sliderFogDensity );
		const QSignalBlocker blockerChromaticGroup( _groupboxChromaticAberration );
		const QSignalBlocker blocker19( _sliderChromaticAberrationRed );
		const QSignalBlocker blocker20( _sliderChromaticAberrationGreen );
		const QSignalBlocker blocker21( _sliderChromaticAberrationBlue );
		const QSignalBlocker blocker22( _groupboxPixelize );
		const QSignalBlocker blocker23( _sliderPixelizeSize );
		const QSignalBlocker blocker24( _checkBoxPixelizeBackground );
		const QSignalBlocker blocker25( _groupboxCRT );
		const QSignalBlocker blocker26( _sliderCRTCurvatureX );
		const QSignalBlocker blocker27( _sliderCRTCurvatureY );
		const QSignalBlocker blocker28( _sliderCRTRatio );
		const QSignalBlocker blocker29( _sliderCRTGraninessX );
		const QSignalBlocker blocker30( _sliderCRTGraninessY );
		const QSignalBlocker blocker31( _sliderCRTVignetteRoundness );
		const QSignalBlocker blocker32( _sliderCRTVignetteIntensity );
		const QSignalBlocker blocker33( _sliderCRTBrightness );
		const QSignalBlocker blocker34( _groupboxSelection );
		const QSignalBlocker blocker35( _colorPickerSelection );

		const Renderer::SSAOConfig	  ssao	  = preset.ssao.value_or( Renderer::GraphicsConfigs::SSAO_DEFAULT );
		const Renderer::OutlineConfig outline = preset.outline.value_or( Renderer::GraphicsConfigs::OUTLINE_DEFAULT );
		const Renderer::FogConfig	  fog	  = preset.fog.value_or( Renderer::GraphicsConfigs::FOG_DEFAULT );
		const Renderer::ChromaticAberrationConfig chromatic
			= preset.chromaticAberration.value_or( Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT );
		const Renderer::PixelizeConfig pixelize
			= preset.pixelize.value_or( Renderer::GraphicsConfigs::PIXELIZE_DEFAULT );
		const Renderer::CRTConfig		crt = preset.crt.value_or( Renderer::GraphicsConfigs::CRT_DEFAULT );
		const Renderer::SelectionConfig selection
			= preset.selection.value_or( Renderer::GraphicsConfigs::SELECTION_DEFAULT );

		_comboBoxShadingMode->setCurrentIndex( int( preset.shading.mode ) );
		_colorPickerBackground->setColor( Helper::toQColor( preset.shading.colorBackground ) );
		_colorPickerLight->setColor( Helper::toQColor( preset.shading.colorLight ) );
		_sliderLightIntensity->setValue( preset.shading.lightIntensity );
		_sliderAmbientIntensity->setValue( preset.shading.ambientIntensity );
		_comboBoxToneMappingMode->setCurrentIndex( int( preset.toneMapping.mode ) );
		_sliderToneMappingExposure->setValue( preset.toneMapping.exposure );
		_sliderSpecularFactor->setValue( preset.shading.specularFactor );
		_sliderShininess->setValue( preset.shading.shininess );
		_sliderToonSteps->setValue( preset.shading.toonSteps );
		_listMaterials->setCurrentRow( -1 );
		for ( int i = 0; i < _listMaterials->count(); ++i )
		{
			const Entity materialPreset = _listMaterials->item( i )->data( Qt::UserRole ).value<Entity>();
			if ( App::REG().get<Renderer::Material>( materialPreset ).name == preset.shading.material.name )
			{
				_listMaterials->setCurrentRow( i );
				break;
			}
		}
		_sliderMaterialMetallic->setValue( preset.shading.material.metallic );
		_sliderMaterialRoughness->setValue( preset.shading.material.roughness );
		_sliderMaterialEmissiveIntensity->setValue( preset.shading.material.emissiveIntensity );
		const QString environmentPath = preset.shading.environmentPath
											? QString::fromStdString( preset.shading.environmentPath->string() )
											: QString {};
		_lineEnvironmentPath->setText( environmentPath );
		_lineEnvironmentPath->setToolTip( environmentPath );
		_buttonEnvironmentClear->setEnabled( preset.shading.environmentPath.has_value() );
		_sliderEnvironmentRotation->setValue( Util::Math::degrees( preset.shading.environmentRotation ) );
		_groupboxSSAO->setChecked( preset.ssao.has_value() );
		_comboBoxSSAOMethod->setCurrentIndex( int( ssao.method ) );
		_comboBoxSSAOScale->setCurrentIndex( _ssaoScaleToUi( ssao.scale ) );
		_sliderSSAOIntensity->setValue( ssao.intensity );
		_sliderBlurSize->setValue( ssao.blurSize );
		_groupboxOutline->setChecked( preset.outline.has_value() );
		_colorPickerOutline->setColor( Helper::toQColor( outline.color ) );
		_sliderOutlineSensitivity->setValue( outline.sensitivity );
		_sliderOutlineThickness->setValue( outline.thickness );
		_groupboxFog->setChecked( preset.fog.has_value() );
		_colorPickerFog->setColor( Helper::toQColor( fog.color ) );
		_sliderFogNear->setValue( fog.near );
		_sliderFogFar->setValue( fog.far );
		_sliderFogDensity->setValue( fog.density );
		_groupboxChromaticAberration->setChecked( preset.chromaticAberration.has_value() );
		_sliderChromaticAberrationRed->setValue( _chromaticAberrationToUi( chromatic.red ) );
		_sliderChromaticAberrationGreen->setValue( _chromaticAberrationToUi( chromatic.green ) );
		_sliderChromaticAberrationBlue->setValue( _chromaticAberrationToUi( chromatic.blue ) );
		_groupboxPixelize->setChecked( preset.pixelize.has_value() );
		_sliderPixelizeSize->setValue( pixelize.size );
		_checkBoxPixelizeBackground->setChecked( pixelize.background );
		_groupboxCRT->setChecked( preset.crt.has_value() );
		_sliderCRTCurvatureX->setValue( crt.curvatureX );
		_sliderCRTCurvatureY->setValue( crt.curvatureY );
		_sliderCRTRatio->setValue( crt.ratio );
		_sliderCRTGraninessX->setValue( crt.graninessX );
		_sliderCRTGraninessY->setValue( crt.graninessY );
		_sliderCRTVignetteRoundness->setValue( crt.vignetteRoundness );
		_sliderCRTVignetteIntensity->setValue( crt.vignetteIntensity );
		_sliderCRTBrightness->setValue( crt.brightness );
		_groupboxSelection->setChecked( preset.selection.has_value() );
		_colorPickerSelection->setColor( Helper::toQColor( selection.color ) );

		_applyLogic( preset );
	}

	void GraphicsConfig::_applyLogic( const Renderer::GraphicsConfig & p_preset )
	{
		using namespace Renderer;
		const bool pbr			  = p_preset.shading.mode == E_SHADING::PBR;
		const bool hasEnvironment = p_preset.shading.environmentPath.has_value();
		_labelEnvironmentRotation->setVisible( hasEnvironment );
		_sliderEnvironmentRotation->setVisible( hasEnvironment );
		_labelLightIntensity->setVisible( pbr );
		_sliderLightIntensity->setVisible( pbr );
		_labelAmbientIntensity->setVisible( pbr );
		_sliderAmbientIntensity->setVisible( pbr );
		_labelToneMappingMode->setVisible( pbr );
		_comboBoxToneMappingMode->setVisible( pbr );
		_labelToneMappingExposure->setVisible( pbr );
		_sliderToneMappingExposure->setVisible( pbr );
		_labelMaterial->setVisible( pbr );
		_listMaterials->setVisible( pbr );
		_labelMaterialMetallic->setVisible( pbr );
		_sliderMaterialMetallic->setVisible( pbr );
		_labelMaterialRoughness->setVisible( pbr );
		_sliderMaterialRoughness->setVisible( pbr );
		_labelMaterialEmissiveIntensity->setVisible( pbr );
		_sliderMaterialEmissiveIntensity->setVisible( pbr );

		switch ( p_preset.shading.mode )
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
		case E_SHADING::PBR:
			_labelSpecularFactor->setVisible( false );
			_sliderSpecularFactor->setVisible( false );
			_labelShininess->setVisible( false );
			_sliderShininess->setVisible( false );
			_labelToonSteps->setVisible( false );
			_sliderToonSteps->setVisible( false );
			break;
		}
	}
} // namespace VTX::UI::QT::Widget::Library
