#include "app/component/representation/representation.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <vector>

namespace VTX::App::Component::Representation
{
	Representation::Representation()
	{
		// TODO: move default layout.
		_representation.hasSphere		  = true;
		_representation.radiusSphereFixed = 0.4f;
		_representation.radiusSphereAdd	  = 0.f;
		_representation.radiusFixed		  = false;

		_representation.hasCylinder			  = true;
		_representation.radiusCylinder		  = 0.1f;
		_representation.cylinderColorBlending = false;

		_representation.hasRibbon			= true;
		_representation.ribbonColorBlending = true;

		_representation.hasSes = false;
	}

	void Representation::setupProxy()
	{
		using namespace Renderer::Proxy;

		_proxy = std::make_unique<Renderer::Proxy::Representation>();

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_SPHERE, _representation.hasSphere );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, _representation.radiusSphereFixed );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, _representation.radiusSphereAdd );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_FIXED, _representation.radiusFixed );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_CYLINDER, _representation.hasCylinder );
		_proxy->set( E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, _representation.radiusCylinder );
		_proxy->set( E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, _representation.cylinderColorBlending );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_RIBBON, _representation.hasRibbon );
		_proxy->set( E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, _representation.ribbonColorBlending );

		_proxy->set( E_REPRESENTATION_SETTINGS::HAS_SES, _representation.hasSes );

		RENDERER_SYSTEM().onReady() += [ this ]()
		{
			std::vector<Renderer::Proxy::Representation *> rep = { _proxy.get() };
			RENDERER_SYSTEM().addProxyRepresentations( rep );
		};
	}

} // namespace VTX::App::Component::Representation
