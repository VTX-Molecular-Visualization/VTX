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
		_representation.shadingMode	  = uint( SHADING_MODE_DEFAULT );
		_representation.colorLight	  = COLOR_LIGHT_DEFAULT;
		_representation.colorBackground = COLOR_BACKGROUND_DEFAULT;
		_representation.specularFactor  = SPECULAR_FACTOR_DEFAULT;
		_representation.shininess		  = SHININESS_DEFAULT;
		_representation.toonSteps		  = TOON_STEPS_DEFAULT;

		_representation.activeSSAO	= ACTIVE_SSAO_DEFAULT;
		_representation.ssaoIntensity = SSAO_INTENSITY_DEFAULT;
		_representation.blurSize		= BLUR_SIZE_DEFAULT;

		_representation.activeOutline		 = ACTIVE_OUTLINE_DEFAULT;
		_representation.colorOutline		 = COLOR_OUTLINE_DEFAULT;
		_representation.outlineSensitivity = OUTLINE_SENSITIVITY_DEFAULT;
		_representation.outlineThickness	 = OUTLINE_THICKNESS_DEFAULT;

		_representation.activeFog	 = ACTIVE_FOG_DEFAULT;
		_representation.colorFog	 = COLOR_FOG_DEFAULT;
		_representation.fogNear	 = FOG_NEAR_DEFAULT;
		_representation.fogFar	 = FOG_FAR_DEFAULT;
		_representation.fogDensity = FOG_DENSITY_DEFAULT;

		_representation.activeSelection = ACTIVE_SELECTION_DEFAULT;
		_representation.colorSelection  = COLOR_SELECTION_DEFAULT;
	}

	void RenderSettings::setupProxy()
	{
		using namespace Renderer::Proxy;

		_proxy = std::make_unique<Renderer::Proxy::RenderSettings>();

		_proxy->set( E_RENDER_SETTINGS::SHADING_MODE, _representation.shadingMode );
		_proxy->set( E_RENDER_SETTINGS::COLOR_LIGHT, _representation.colorLight );
		_proxy->set( E_RENDER_SETTINGS::COLOR_BACKGROUND, _representation.colorBackground );
		_proxy->set( E_RENDER_SETTINGS::SPECULAR_FACTOR, _representation.specularFactor );
		_proxy->set( E_RENDER_SETTINGS::SHININESS, _representation.shininess );
		_proxy->set( E_RENDER_SETTINGS::TOON_STEPS, _representation.toonSteps );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_SSAO, _representation.activeSSAO );
		_proxy->set( E_RENDER_SETTINGS::SSAO_INTENSITY, _representation.ssaoIntensity );
		_proxy->set( E_RENDER_SETTINGS::BLUR_SIZE, _representation.blurSize );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_OUTLINE, _representation.activeOutline );
		_proxy->set( E_RENDER_SETTINGS::COLOR_OUTLINE, _representation.colorOutline );
		_proxy->set( E_RENDER_SETTINGS::OUTLINE_SENSITIVITY, _representation.outlineSensitivity );
		_proxy->set( E_RENDER_SETTINGS::OUTLINE_THICKNESS, _representation.outlineThickness );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_FOG, _representation.activeFog );
		_proxy->set( E_RENDER_SETTINGS::COLOR_FOG, _representation.colorFog );
		_proxy->set( E_RENDER_SETTINGS::FOG_NEAR, _representation.fogNear );
		_proxy->set( E_RENDER_SETTINGS::FOG_FAR, _representation.fogFar );
		_proxy->set( E_RENDER_SETTINGS::FOG_DENSITY, _representation.fogDensity );

		_proxy->set( E_RENDER_SETTINGS::ACTIVE_SELECTION, _representation.activeSelection );
		_proxy->set( E_RENDER_SETTINGS::COLOR_SELECTION, _representation.colorSelection );

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
