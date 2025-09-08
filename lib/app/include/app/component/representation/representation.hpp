#ifndef __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__
#define __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__

#include "app/core/ecs/base_component.hpp"
#include "app/library/preset/representation.hpp"
#include <renderer/proxy/representation.hpp>

namespace VTX::App::Component::Representation
{
	// !!! un proxy par rep utilisee donc un component !
	class Representation :
		public Core::ECS::
			BaseComponentProxyPreset<Renderer::Proxy::Representation, App::Library::Preset::Representation>
	{
	  public:
		Representation() = delete;
		Representation( Library::Preset::Representation & p_preset );

		void setupProxy() override;
	};

} // namespace VTX::App::Component::Representation

#endif
