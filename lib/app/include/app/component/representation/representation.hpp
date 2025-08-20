#ifndef __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__
#define __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__

#include "app/core/ecs/base_component.hpp"
#include <core/struct/representation.hpp>
#include <renderer/proxy/representation.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{
	// !!! un proxy par rep utilisee donc un component !
	class Representation : public Core::ECS::BaseComponentProxy<Renderer::Proxy::Representation>
	{
	  public:
		Representation();

		void setupProxy() override;

		// const VTX::Core::Struct::Representation & getRepresentation() const { return _representation; }

		// template<Renderer::Proxy::E_REPRESENTATION_VALUES S, typename T>
		// void set( const T p_value )
		//{
		//  TODO: check presets for modification then update the rep on gpu.
		//  No more model there?
		//  Move enum to core?

		// callback<S, T>()( p_value );

		// Trigger renderer.

		//_proxy->onChange<S, T>()( p_value );
		//}

	  private:
		// VTX::Core::Struct::Representation _representation;
	};

} // namespace VTX::App::Component::Representation

#endif
