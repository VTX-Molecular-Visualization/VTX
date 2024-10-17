#include "app/component/representation/render_settings.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <util/logger.hpp>

namespace VTX::App::Component::Representation
{
	RenderSettings::RenderSettings()
	{
		// TODO: where to set default values?
		// setColors( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL.layout );
	}

	void RenderSettings::setupProxy()
	{
		_proxy = std::make_unique<Renderer::Proxy::RenderSettings>( Renderer::Proxy::RenderSettings {
			_settings.ssaoIntensity,
			_settings.blurSize,
			_settings.colorBackground,
			_settings.colorLight,
			_settings.colorFog,
			_settings.shadingMode,
			_settings.specularFactor,
			_settings.shininess,
			_settings.toonSteps,
			_settings.fogNear,
			_settings.fogFar,
			_settings.fogDensity,
			_settings.colorOutline,
			_settings.outlineSensitivity,
			_settings.outlineThickness,
			_settings.colorSelection,
		} );

		// onChange += [ this ]( const size_t ) { _proxy->onChange(); };
		// onChangeAll += [ this ]() { _proxy->onChange(); };

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
