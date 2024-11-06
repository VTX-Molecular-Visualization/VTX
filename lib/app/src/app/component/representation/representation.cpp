#include "app/component/representation/representation.hpp"
#include "app/core/renderer/renderer_system.hpp"

namespace VTX::App::Component::Representation
{
	Representation::Representation()
	{
		// TODO: move default layout.
	}

	void Representation::setupProxy()
	{
		/*
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

			*/
		RENDERER_SYSTEM().onReady() += [ this ]() { RENDERER_SYSTEM().addProxyRepresentation( *_proxy ); };
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
