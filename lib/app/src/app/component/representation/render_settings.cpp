#include "app/component/representation/render_settings.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <renderer/proxy/voxels.hpp>
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
		using namespace Renderer::Proxy;

		_proxy = std::make_unique<Renderer::Proxy::RenderSettings>();

		_proxy->set( E_RENDER_SETTINGS::SHADING_MODE, _settings.shadingMode );
		_proxy->set( E_RENDER_SETTINGS::COLOR_LIGHT, _settings.colorLight );
		_proxy->set( E_RENDER_SETTINGS::COLOR_BACKGROUND, _settings.colorBackground );
		_proxy->set( E_RENDER_SETTINGS::SPECULAR_FACTOR, _settings.specularFactor );
		_proxy->set( E_RENDER_SETTINGS::SHININESS, _settings.shininess );
		_proxy->set( E_RENDER_SETTINGS::TOON_STEPS, _settings.toonSteps );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_SSAO, _settings.activeSSAO );
		_proxy->set( E_RENDER_SETTINGS::SSAO_INTENSITY, _settings.ssaoIntensity );
		_proxy->set( E_RENDER_SETTINGS::BLUR_SIZE, _settings.blurSize );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_OUTLINE, _settings.activeOutline );
		_proxy->set( E_RENDER_SETTINGS::COLOR_OUTLINE, _settings.colorOutline );
		_proxy->set( E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, _settings.outlineSensitivity );
		_proxy->set( E_RENDER_SETTINGS::OUTLINE_THICKNESS, _settings.outlineThickness );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_FOG, _settings.activeFog );
		_proxy->set( E_RENDER_SETTINGS::COLOR_FOG, _settings.colorFog );
		_proxy->set( E_RENDER_SETTINGS::FOG_NEAR, _settings.fogNear );
		_proxy->set( E_RENDER_SETTINGS::FOG_FAR, _settings.fogFar );
		_proxy->set( E_RENDER_SETTINGS::FOG_DENSITY, _settings.fogDensity );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_SELECTION, _settings.activeSelection );
		_proxy->set( E_RENDER_SETTINGS::COLOR_SELECTION, _settings.colorSelection );

		RENDERER_SYSTEM().onReady() += [ this ]() { RENDERER_SYSTEM().setProxyRenderSettings( *_proxy ); };

		/*
		RENDERER_SYSTEM().onReady() += [ & ]()
		{
			std::vector<Vec3f> mins, maxs;
			for ( float x = -100.f; x <= 100.f; x += 50.f )
			{
				for ( float y = -100.f; y <= 100.f; y += 50.f )
				{
					for ( float z = -100.f; z <= 100.f; z += 50.f )
					{
						mins.emplace_back( x, y, z );
						maxs.emplace_back( x + 50.f, y + 50.f, z + 50.f );
					}
				}
			}
			auto proxyVoxels = Renderer::Proxy::Voxels { &mins, &maxs };
			RENDERER_SYSTEM().setProxyVoxels( proxyVoxels );
		};
		*/
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
