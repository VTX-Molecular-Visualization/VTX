#include "app/python_binding/preset/graphics_config.hpp"
#include "app/action/graphics_config.hpp"
#include "app/action/scene.hpp"
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
		{
			validate( p_preset );
			executeAction<Action::GraphicsConfig::Change<VALUE, T>>( Entity( p_preset.entity ), T( p_value ) );
		}

		template<Renderer::E_GRAPHICS_CONFIG_VALUES VALUE, typename T, typename Getter>
		void _bindProperty(
			pybind11::class_<GraphicsConfigHandle> & p_binding,
			const char * const						 p_name,
			const Getter							 p_getter
		)
		{ p_binding.def_property( p_name, p_getter, &_setValue<VALUE, T> ); }

		void _setEnvironmentPath( const GraphicsConfigHandle & p_preset, const std::optional<std::string> & p_path )
		{
			validate( p_preset );
			executeAction<
				Action::GraphicsConfig::Change<Renderer::E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH, FilePath>>(
				Entity( p_preset.entity ), FilePath( p_path.value_or( "" ) )
			);
		}

		const Renderer::SSAOConfig & _getSsao( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).ssao;
			return config ? *config : Renderer::GraphicsConfigs::SSAO_DEFAULT;
		}

		const Renderer::FogConfig & _getFog( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).fog;
			return config ? *config : Renderer::GraphicsConfigs::FOG_DEFAULT;
		}

		const Renderer::OutlineConfig & _getOutline( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).outline;
			return config ? *config : Renderer::GraphicsConfigs::OUTLINE_DEFAULT;
		}

		const Renderer::ChromaticAberrationConfig & _getChromaticAberration( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).chromaticAberration;
			return config ? *config : Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT;
		}

		const Renderer::PixelizeConfig & _getPixelize( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).pixelize;
			return config ? *config : Renderer::GraphicsConfigs::PIXELIZE_DEFAULT;
		}

		const Renderer::CRTConfig & _getCrt( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).crt;
			return config ? *config : Renderer::GraphicsConfigs::CRT_DEFAULT;
		}

		const Renderer::SelectionConfig & _getSelection( const GraphicsConfigHandle & p_preset )
		{
			const auto & config = getData( p_preset ).selection;
			return config ? *config : Renderer::GraphicsConfigs::SELECTION_DEFAULT;
		}

		void _apply( const GraphicsConfigHandle & p_preset )
		{
			validate( p_preset );
			executeAction<Action::Scene::SetGraphicsConfig>( Entity( p_preset.entity ) );
		}
	} // namespace

	void bindGraphicsConfig( Module & p_vtxModule, pybind11::class_<GraphicsConfigHandle> & p_binding )
	{
		using Value = Renderer::E_GRAPHICS_CONFIG_VALUES;

		VTX::PythonBinding::Helper::declareEnum<Value>( p_vtxModule.pyModule(), "GRAPHICS_CONFIG_VALUE" );
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_SHADING>( p_vtxModule.pyModule(), "SHADING_MODE" );
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_BACKGROUND_MODE>(
			p_vtxModule.pyModule(), "BACKGROUND_MODE"
		);
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_SSAO_METHOD>( p_vtxModule.pyModule(), "SSAO_METHOD" );
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_TONE_MAPPING>( p_vtxModule.pyModule(), "TONE_MAPPING" );

		_bindProperty<Value::SHADING_MODE, Renderer::E_SHADING>(
			p_binding, "shadingMode", []( const GraphicsConfigHandle & p ) { return getData( p ).shading.mode; }
		);
		_bindProperty<Value::COLOR_LIGHT, Util::Color::Rgba>(
			p_binding, "colorLight", []( const GraphicsConfigHandle & p ) { return getData( p ).shading.colorLight; }
		);
		_bindProperty<Value::LIGHT_INTENSITY, float>(
			p_binding,
			"lightIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.lightIntensity; }
		);
		_bindProperty<Value::AMBIENT_INTENSITY, float>(
			p_binding,
			"ambientIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.ambientIntensity; }
		);
		_bindProperty<Value::BACKGROUND_MODE, Renderer::E_BACKGROUND_MODE>(
			p_binding,
			"backgroundMode",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.backgroundMode; }
		);
		_bindProperty<Value::COLOR_BACKGROUND, Util::Color::Rgba>(
			p_binding,
			"colorBackground",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.colorBackground; }
		);
		_bindProperty<Value::SPECULAR_FACTOR, float>(
			p_binding,
			"specularFactor",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.specularFactor; }
		);
		_bindProperty<Value::SHININESS, float>(
			p_binding, "shininess", []( const GraphicsConfigHandle & p ) { return getData( p ).shading.shininess; }
		);
		_bindProperty<Value::TOON_STEPS, uint>(
			p_binding, "toonSteps", []( const GraphicsConfigHandle & p ) { return getData( p ).shading.toonSteps; }
		);
		p_binding.def_property(
			"environmentPath",
			[]( const GraphicsConfigHandle & p_preset ) -> std::optional<std::string>
			{
				const std::optional<FilePath> & path = getData( p_preset ).shading.environmentPath;
				return path ? std::optional<std::string>( path->string() ) : std::nullopt;
			},
			&_setEnvironmentPath
		);
		_bindProperty<Value::SKYBOX_INTENSITY, float>(
			p_binding,
			"skyboxIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.skyboxIntensity; }
		);
		_bindProperty<Value::IBL_INTENSITY, float>(
			p_binding,
			"iblIntensity",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.iblIntensity; }
		);
		_bindProperty<Value::ENVIRONMENT_ROTATION, float>(
			p_binding,
			"environmentRotation",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).shading.environmentRotation; }
		);
		_bindProperty<Value::TONE_MAPPING_MODE, Renderer::E_TONE_MAPPING>(
			p_binding, "toneMappingMode", []( const GraphicsConfigHandle & p ) { return getData( p ).toneMapping.mode; }
		);
		_bindProperty<Value::TONE_MAPPING_EXPOSURE, float>(
			p_binding,
			"toneMappingExposure",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).toneMapping.exposure; }
		);

		_bindProperty<Value::ACTIVE_SSAO, bool>(
			p_binding, "activeSsao", []( const GraphicsConfigHandle & p ) { return getData( p ).ssao.has_value(); }
		);
		_bindProperty<Value::SSAO_METHOD, Renderer::E_SSAO_METHOD>(
			p_binding, "ssaoMethod", []( const GraphicsConfigHandle & p ) { return _getSsao( p ).method; }
		);
		_bindProperty<Value::SSAO_INTENSITY, float>(
			p_binding, "ssaoIntensity", []( const GraphicsConfigHandle & p ) { return _getSsao( p ).intensity; }
		);
		_bindProperty<Value::SSAO_SCALE, float>(
			p_binding, "ssaoScale", []( const GraphicsConfigHandle & p ) { return _getSsao( p ).scale; }
		);
		_bindProperty<Value::BLUR_SIZE, float>(
			p_binding, "blurSize", []( const GraphicsConfigHandle & p ) { return _getSsao( p ).blurSize; }
		);

		_bindProperty<Value::ACTIVE_FOG, bool>(
			p_binding, "activeFog", []( const GraphicsConfigHandle & p ) { return getData( p ).fog.has_value(); }
		);
		_bindProperty<Value::COLOR_FOG, Util::Color::Rgba>(
			p_binding, "colorFog", []( const GraphicsConfigHandle & p ) { return _getFog( p ).color; }
		);
		_bindProperty<Value::FOG_NEAR, float>(
			p_binding, "fogNear", []( const GraphicsConfigHandle & p ) { return _getFog( p ).near; }
		);
		_bindProperty<Value::FOG_FAR, float>(
			p_binding, "fogFar", []( const GraphicsConfigHandle & p ) { return _getFog( p ).far; }
		);
		_bindProperty<Value::FOG_DENSITY, float>(
			p_binding, "fogDensity", []( const GraphicsConfigHandle & p ) { return _getFog( p ).density; }
		);

		_bindProperty<Value::ACTIVE_OUTLINE, bool>(
			p_binding,
			"activeOutline",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).outline.has_value(); }
		);
		_bindProperty<Value::COLOR_OUTLINE, Util::Color::Rgba>(
			p_binding, "colorOutline", []( const GraphicsConfigHandle & p ) { return _getOutline( p ).color; }
		);
		_bindProperty<Value::OUTLINE_SENSITIVITY, float>(
			p_binding,
			"outlineSensitivity",
			[]( const GraphicsConfigHandle & p ) { return _getOutline( p ).sensitivity; }
		);
		_bindProperty<Value::OUTLINE_THICKNESS, uint>(
			p_binding, "outlineThickness", []( const GraphicsConfigHandle & p ) { return _getOutline( p ).thickness; }
		);

		_bindProperty<Value::ACTIVE_CHROMAB, bool>(
			p_binding,
			"activeChromaticAberration",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).chromaticAberration.has_value(); }
		);
		_bindProperty<Value::CHROMAB_RED, float>(
			p_binding,
			"chromaticAberrationRed",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).red; }
		);
		_bindProperty<Value::CHROMAB_GREEN, float>(
			p_binding,
			"chromaticAberrationGreen",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).green; }
		);
		_bindProperty<Value::CHROMAB_BLUE, float>(
			p_binding,
			"chromaticAberrationBlue",
			[]( const GraphicsConfigHandle & p ) { return _getChromaticAberration( p ).blue; }
		);

		_bindProperty<Value::ACTIVE_PIXELIZE, bool>(
			p_binding,
			"activePixelize",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).pixelize.has_value(); }
		);
		_bindProperty<Value::PIXELIZE_SIZE, uint>(
			p_binding, "pixelizeSize", []( const GraphicsConfigHandle & p ) { return _getPixelize( p ).size; }
		);
		_bindProperty<Value::PIXELIZE_BACKGROUND, bool>(
			p_binding,
			"pixelizeBackground",
			[]( const GraphicsConfigHandle & p ) { return _getPixelize( p ).background; }
		);

		_bindProperty<Value::ACTIVE_CRT, bool>(
			p_binding, "activeCrt", []( const GraphicsConfigHandle & p ) { return getData( p ).crt.has_value(); }
		);
		_bindProperty<Value::CRT_CURVATURE_X, float>(
			p_binding, "crtCurvatureX", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).curvatureX; }
		);
		_bindProperty<Value::CRT_CURVATURE_Y, float>(
			p_binding, "crtCurvatureY", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).curvatureY; }
		);
		_bindProperty<Value::CRT_RATIO, float>(
			p_binding, "crtRatio", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).ratio; }
		);
		_bindProperty<Value::CRT_GRANINESS_X, float>(
			p_binding, "crtGraninessX", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).graninessX; }
		);
		_bindProperty<Value::CRT_GRANINESS_Y, float>(
			p_binding, "crtGraninessY", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).graninessY; }
		);
		_bindProperty<Value::CRT_VIGNETTE_ROUNDNESS, float>(
			p_binding,
			"crtVignetteRoundness",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).vignetteRoundness; }
		);
		_bindProperty<Value::CRT_VIGNETTE_INTENSITY, float>(
			p_binding,
			"crtVignetteIntensity",
			[]( const GraphicsConfigHandle & p ) { return _getCrt( p ).vignetteIntensity; }
		);
		_bindProperty<Value::CRT_BRIGHTNESS, float>(
			p_binding, "crtBrightness", []( const GraphicsConfigHandle & p ) { return _getCrt( p ).brightness; }
		);

		_bindProperty<Value::ACTIVE_SELECTION, bool>(
			p_binding,
			"activeSelection",
			[]( const GraphicsConfigHandle & p ) { return getData( p ).selection.has_value(); }
		);
		_bindProperty<Value::COLOR_SELECTION, Util::Color::Rgba>(
			p_binding, "colorSelection", []( const GraphicsConfigHandle & p ) { return _getSelection( p ).color; }
		);

		p_binding.def( "apply", &_apply, "Apply this graphics config preset to the scene." );
	}
} // namespace VTX::App::PythonBinding::Preset
