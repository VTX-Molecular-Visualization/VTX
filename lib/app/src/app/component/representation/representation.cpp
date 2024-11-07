#include "app/component/representation/representation.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <vector>

namespace VTX::App::Component::Representation
{
	Representation::Representation()
	{
		// TODO: move default layout.
		_settings.hasSphere			= true;
		_settings.radiusSphereFixed = 0.4f;
		_settings.radiusSphereAdd	= 0.f;
		_settings.radiusFixed		= true;

		_settings.hasCylinder			= true;
		_settings.radiusCylinder		= 0.1f;
		_settings.cylinderColorBlending = false;

		_settings.hasRibbon			  = true;
		_settings.ribbonColorBlending = true;

		_settings.hasSes = false;
	}

	void Representation::setupProxy()
	{
		using namespace Renderer::Proxy;

		_proxy = std::make_unique<Renderer::Proxy::Representation>();

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_SPHERE, _settings.hasSphere );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, _settings.radiusSphereFixed );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, _settings.radiusSphereAdd );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_FIXED, _settings.radiusFixed );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_CYLINDER, _settings.hasCylinder );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, _settings.radiusCylinder );
		_proxy->set( E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, _settings.cylinderColorBlending );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_RIBBON, _settings.hasRibbon );
		_proxy->set( E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, _settings.ribbonColorBlending );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_SES, _settings.hasSes );

		RENDERER_SYSTEM().onReady() += [ this ]()
		{
			std::vector<Renderer::Proxy::Representation *> rep = { _proxy.get() };
			RENDERER_SYSTEM().addProxyRepresentations( rep );
		};
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
