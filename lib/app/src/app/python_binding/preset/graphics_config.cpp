#include "app/python_binding/preset/graphics_config.hpp"
#include "app/action/graphics_config.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include <pybind11/stl.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/graphics_config.hpp>
#include <string>

namespace VTX::App::PythonBinding::Preset
{
	namespace
	{
		template<Renderer::E_GRAPHICS_CONFIG_VALUES VALUE, typename T>
		void _setValue( const GraphicsConfigHandle & p_preset, const T p_value )
		{ executePresetAction<Action::GraphicsConfig::Change<VALUE, T>>( p_preset, T( p_value ) ); }

		template<Renderer::E_GRAPHICS_CONFIG_VALUES VALUE, typename T, typename Getter>
		void _bindProperty(
			pybind11::class_<GraphicsConfigHandle> & p_binding,
			const char * const						 p_name,
			const Getter							 p_getter,
			const char * const						 p_doc
		)
		{ p_binding.def_property( p_name, p_getter, &_setValue<VALUE, T>, p_doc ); }

		void _setEnvironmentPath( const GraphicsConfigHandle & p_preset, const std::optional<std::string> & p_path )
		{
			executePresetAction<
				Action::GraphicsConfig::Change<Renderer::E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>>(
				p_preset, FilePath( p_path.value_or( "" ) )
			);
		}

		Renderer::SSAOConfig _getSsao( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).ssao;
			return config ? *config : Renderer::GraphicsConfigs::SSAO_DEFAULT;
		}

		Renderer::FogConfig _getFog( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).fog;
			return config ? *config : Renderer::GraphicsConfigs::FOG_DEFAULT;
		}

		Renderer::OutlineConfig _getOutline( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).outline;
			return config ? *config : Renderer::GraphicsConfigs::OUTLINE_DEFAULT;
		}

		Renderer::ChromaticAberrationConfig _getChromaticAberration( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).chromaticAberration;
			return config ? *config : Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT;
		}

		Renderer::PixelizeConfig _getPixelize( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).pixelize;
			return config ? *config : Renderer::GraphicsConfigs::PIXELIZE_DEFAULT;
		}

		Renderer::CRTConfig _getCrt( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).crt;
			return config ? *config : Renderer::GraphicsConfigs::CRT_DEFAULT;
		}

		Renderer::SelectionConfig _getSelection( const GraphicsConfigHandle & p_preset )
		{
			const auto config = getData( p_preset ).selection;
			return config ? *config : Renderer::GraphicsConfigs::SELECTION_DEFAULT;
		}

	} // namespace

	void bindGraphicsConfig( Module & p_vtxModule, pybind11::class_<GraphicsConfigHandle> & p_binding )
	{
		using Value = Renderer::E_GRAPHICS_CONFIG_VALUES;

		VTX::PythonBinding::Helper::declareEnum<Renderer::E_SHADING>(
			p_vtxModule.pyModule(), "SHADING_MODE", "Shading mode."
		);
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_BACKGROUND_MODE>(
			p_vtxModule.pyModule(), "BACKGROUND_MODE", "Background mode."
		);
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_SSAO_METHOD>(
			p_vtxModule.pyModule(), "SSAO_METHOD", "Ambient occlusion method."
		);
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_TONE_MAPPING>(
			p_vtxModule.pyModule(), "TONE_MAPPING", "Tone mapping operator."
		);

		_bindProperty<Value::SHADING_MODE, Renderer::E_SHADING>(
			p_binding,
			"shadingMode",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.mode; },
			"Shading mode."
		);
		_bindProperty<Value::COLOR_LIGHT, Util::Color::Rgba>(
			p_binding,
			"colorLight",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.colorLight; },
			"Light color."
		);
		_bindProperty<Value::LIGHT_INTENSITY, float>(
			p_binding,
			"lightIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.lightIntensity; },
			"Light intensity."
		);
		_bindProperty<Value::AMBIENT_INTENSITY, float>(
			p_binding,
			"ambientIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.ambientIntensity; },
			"Ambient light intensity."
		);
		_bindProperty<Value::BACKGROUND_MODE, Renderer::E_BACKGROUND_MODE>(
			p_binding,
			"backgroundMode",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.backgroundMode; },
			"Background mode."
		);
		_bindProperty<Value::COLOR_BACKGROUND, Util::Color::Rgba>(
			p_binding,
			"colorBackground",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.colorBackground; },
			"Background color."
		);
		_bindProperty<Value::SPECULAR_FACTOR, float>(
			p_binding,
			"specularFactor",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.specularFactor; },
			"Specular reflection factor."
		);
		_bindProperty<Value::SHININESS, float>(
			p_binding,
			"shininess",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.shininess; },
			"Specular shininess exponent."
		);
		_bindProperty<Value::TOON_STEPS, uint>(
			p_binding,
			"toonSteps",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.toonSteps; },
			"Number of shading steps in toon mode."
		);
		p_binding.def_property(
			"environmentPath",
			[]( const GraphicsConfigHandle & p_preset ) -> std::optional<std::string>
			{
				const std::optional<FilePath> path = getData( p_preset ).shading.environmentPath;
				return path ? std::optional<std::string>( path->string() ) : std::nullopt;
			},
			&_setEnvironmentPath,
			"Environment map file path, or None."
		);
		_bindProperty<Value::SKYBOX_INTENSITY, float>(
			p_binding,
			"skyboxIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.skyboxIntensity; },
			"Skybox brightness."
		);
		_bindProperty<Value::IBL_INTENSITY, float>(
			p_binding,
			"iblIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.iblIntensity; },
			"Image-based lighting intensity."
		);
		_bindProperty<Value::ENVIRONMENT_ROTATION, float>(
			p_binding,
			"environmentRotation",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.environmentRotation; },
			"Environment map rotation in radians."
		);
		_bindProperty<Value::TONE_MAPPING_MODE, Renderer::E_TONE_MAPPING>(
			p_binding,
			"toneMappingMode",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).toneMapping.mode; },
			"Tone mapping operator."
		);
		_bindProperty<Value::TONE_MAPPING_EXPOSURE, float>(
			p_binding,
			"toneMappingExposure",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).toneMapping.exposure; },
			"Tone mapping exposure."
		);

		_bindProperty<Value::ACTIVE_SSAO, bool>(
			p_binding,
			"activeSsao",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).ssao.has_value(); },
			"Whether ambient occlusion is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::SSAO_METHOD, Renderer::E_SSAO_METHOD>(
			p_binding,
			"ssaoMethod",
			[]( const GraphicsConfigHandle & p ) { return _getSsao( p ).method; },
			"Ambient occlusion method."
		);
		_bindProperty<Value::SSAO_INTENSITY, float>(
			p_binding,
			"ssaoIntensity",
			[]( const GraphicsConfigHandle & p ) { return _getSsao( p ).intensity; },
			"Ambient occlusion intensity."
		);
		_bindProperty<Value::SSAO_SCALE, float>(
			p_binding,
			"ssaoScale",
			[]( const GraphicsConfigHandle & p ) { return _getSsao( p ).scale; },
			"Ambient occlusion scale."
		);
		_bindProperty<Value::BLUR_SIZE, float>(
			p_binding,
			"blurSize",
			[]( const GraphicsConfigHandle & p ) { return _getSsao( p ).blurSize; },
			"Ambient occlusion blur kernel size."
		);

		_bindProperty<Value::ACTIVE_FOG, bool>(
			p_binding,
			"activeFog",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).fog.has_value(); },
			"Whether fog is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::COLOR_FOG, Util::Color::Rgba>(
			p_binding, "colorFog", []( const GraphicsConfigHandle & p ) { return _getFog( p ).color; }, "Fog color."
		);
		_bindProperty<Value::FOG_NEAR, float>(
			p_binding,
			"fogNear",
			[]( const GraphicsConfigHandle & p ) { return _getFog( p ).near; },
			"Distance where the fog starts."
		);
		_bindProperty<Value::FOG_FAR, float>(
			p_binding,
			"fogFar",
			[]( const GraphicsConfigHandle & p ) { return _getFog( p ).far; },
			"Distance where the fog is full."
		);
		_bindProperty<Value::FOG_DENSITY, float>(
			p_binding,
			"fogDensity",
			[]( const GraphicsConfigHandle & p ) { return _getFog( p ).density; },
			"Fog density."
		);

		_bindProperty<Value::ACTIVE_OUTLINE, bool>(
			p_binding,
			"activeOutline",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).outline.has_value(); },
			"Whether the outline is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::COLOR_OUTLINE, Util::Color::Rgba>(
			p_binding,
			"colorOutline",
			[]( const GraphicsConfigHandle & p ) { return _getOutline( p ).color; },
			"Outline color."
		);
		_bindProperty<Value::OUTLINE_SENSITIVITY, float>(
			p_binding,
			"outlineSensitivity",
			[]( const GraphicsConfigHandle & p ) { return _getOutline( p ).sensitivity; },
			"Outline edge detection sensitivity."
		);
		_bindProperty<Value::OUTLINE_THICKNESS, uint>(
			p_binding,
			"outlineThickness",
			[]( const GraphicsConfigHandle & p ) { return _getOutline( p ).thickness; },
			"Outline thickness in pixels."
		);

		_bindProperty<Value::ACTIVE_CHROMAB, bool>(
			p_binding,
			"activeChromaticAberration",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).chromaticAberration.has_value(); },
			"Whether chromatic aberration is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::CHROMAB_RED, float>(
			p_binding,
			"chromaticAberrationRed",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).red; },
			"Red channel offset."
		);
		_bindProperty<Value::CHROMAB_GREEN, float>(
			p_binding,
			"chromaticAberrationGreen",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).green; },
			"Green channel offset."
		);
		_bindProperty<Value::CHROMAB_BLUE, float>(
			p_binding,
			"chromaticAberrationBlue",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).blue; },
			"Blue channel offset."
		);

		_bindProperty<Value::ACTIVE_PIXELIZE, bool>(
			p_binding,
			"activePixelize",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).pixelize.has_value(); },
			"Whether pixelization is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::PIXELIZE_SIZE, uint>(
			p_binding,
			"pixelizeSize",
			[]( const GraphicsConfigHandle & p ) { return _getPixelize( p ).size; },
			"Pixel block size in screen pixels."
		);
		_bindProperty<Value::PIXELIZE_BACKGROUND, bool>(
			p_binding,
			"pixelizeBackground",
			[]( const GraphicsConfigHandle & p ) { return _getPixelize( p ).background; },
			"Whether the background is pixelized too."
		);

		_bindProperty<Value::ACTIVE_CRT, bool>(
			p_binding,
			"activeCrt",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).crt.has_value(); },
			"Whether the CRT effect is enabled. Enabling it resets its settings; setting one of them enables it."
		);
		_bindProperty<Value::CRT_CURVATURE_X, float>(
			p_binding,
			"crtCurvatureX",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).curvatureX; },
			"Horizontal screen curvature."
		);
		_bindProperty<Value::CRT_CURVATURE_Y, float>(
			p_binding,
			"crtCurvatureY",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).curvatureY; },
			"Vertical screen curvature."
		);
		_bindProperty<Value::CRT_RATIO, float>(
			p_binding,
			"crtRatio",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).ratio; },
			"Scanline density."
		);
		_bindProperty<Value::CRT_GRANINESS_X, float>(
			p_binding,
			"crtGraninessX",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).graninessX; },
			"Horizontal grain."
		);
		_bindProperty<Value::CRT_GRANINESS_Y, float>(
			p_binding,
			"crtGraninessY",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).graninessY; },
			"Vertical grain."
		);
		_bindProperty<Value::CRT_VIGNETTE_ROUNDNESS, float>(
			p_binding,
			"crtVignetteRoundness",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).vignetteRoundness; },
			"Vignette roundness."
		);
		_bindProperty<Value::CRT_VIGNETTE_INTENSITY, float>(
			p_binding,
			"crtVignetteIntensity",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).vignetteIntensity; },
			"Vignette intensity."
		);
		_bindProperty<Value::CRT_BRIGHTNESS, float>(
			p_binding,
			"crtBrightness",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).brightness; },
			"Brightness."
		);

		_bindProperty<Value::ACTIVE_SELECTION, bool>(
			p_binding,
			"activeSelection",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).selection.has_value(); },
			"Whether the selection highlight is enabled. Enabling it resets its settings; setting one of them enables "
			"it."
		);
		_bindProperty<Value::COLOR_SELECTION, Util::Color::Rgba>(
			p_binding,
			"colorSelection",
			[]( const GraphicsConfigHandle & p ) { return _getSelection( p ).color; },
			"Selection highlight color."
		);
	}
} // namespace VTX::App::PythonBinding::Preset
