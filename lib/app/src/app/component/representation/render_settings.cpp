#include "app/component/representation/render_settings.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <renderer/settings.hpp>
#include <util/logger.hpp>

namespace VTX::App::Component::Representation
{
	RenderSettings::RenderSettings()
	{
		using namespace Renderer;

		// TODO: move default layout.
		_settings.shadingMode	  = uint( SHADING_MODE_DEFAULT );
		_settings.colorLight	  = COLOR_LIGHT_DEFAULT;
		_settings.colorBackground = COLOR_BACKGROUND_DEFAULT;
		_settings.specularFactor  = SPECULAR_FACTOR_DEFAULT;
		_settings.shininess		  = SHININESS_DEFAULT;
		_settings.toonSteps		  = TOON_STEPS_DEFAULT;

		_settings.activeSSAO	= ACTIVE_SSAO_DEFAULT;
		_settings.ssaoIntensity = SSAO_INTENSITY_DEFAULT;
		_settings.blurSize		= BLUR_SIZE_DEFAULT;

		_settings.activeOutline		 = ACTIVE_OUTLINE_DEFAULT;
		_settings.colorOutline		 = COLOR_OUTLINE_DEFAULT;
		_settings.outlineSensitivity = OUTLINE_SENSITIVITY_DEFAULT;
		_settings.outlineThickness	 = OUTLINE_THICKNESS_DEFAULT;

		_settings.activeFog	 = ACTIVE_FOG_DEFAULT;
		_settings.colorFog	 = COLOR_FOG_DEFAULT;
		_settings.fogNear	 = FOG_NEAR_DEFAULT;
		_settings.fogFar	 = FOG_FAR_DEFAULT;
		_settings.fogDensity = FOG_DENSITY_DEFAULT;

		_settings.activeSelection = ACTIVE_SELECTION_DEFAULT;
		_settings.colorSelection  = COLOR_SELECTION_DEFAULT;
	}

	void RenderSettings::setupProxy()
	{
		_proxy = std::make_unique<Renderer::Proxy::RenderSettings>( Renderer::Proxy::RenderSettings {
			//
			_settings.shadingMode,
			_settings.colorLight,
			_settings.colorBackground,
			_settings.specularFactor,
			_settings.shininess,
			_settings.toonSteps,
			//
			_settings.activeSSAO,
			_settings.ssaoIntensity,
			_settings.blurSize,
			//
			_settings.activeOutline,
			_settings.colorOutline,
			_settings.outlineSensitivity,
			_settings.outlineThickness,
			//
			_settings.activeFog,
			_settings.colorFog,
			_settings.fogNear,
			_settings.fogFar,
			_settings.fogDensity,
			//
			_settings.activeSelection,
			_settings.colorSelection } );

		RENDERER_SYSTEM().onReady() += [ this ]() { RENDERER_SYSTEM().setProxyRenderSettings( *_proxy ); };
	}

	/*
	void ColorLayout::setColor( const size_t p_index, const Util::Color::Rgba & p_color )
	{
		assert( p_index >= 0 );
		assert( p_index < VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );

		_layout.layout[ p_index ] = p_color;
		onChange( p_index );
	}
	void ColorLayout::setColors( const std::vector<Util::Color::Rgba> & p_colors )
	{
		assert( p_colors.size() == VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );

		_layout.layout = p_colors;
		onChangeAll();
	}
	*/

} // namespace VTX::App::Component::Representation
