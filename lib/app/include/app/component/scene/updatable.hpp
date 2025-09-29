#ifndef __VTX_APP_COMPONENT_SCENE_UPDATABLE__
#define __VTX_APP_COMPONENT_SCENE_UPDATABLE__

#include "app/core/ecs/base_component.hpp"
#include "app/events.hpp"
#include <functional>
#include <util/callback.hpp>
#include <util/event_hub.hpp>

namespace VTX::App::Component::Scene
{
	using UpdateFunction = std::function<void( const Events::Update & )>;

	class Updatable : public Core::ECS::BaseComponent
	{
	  public:
		Updatable() = default;
		virtual ~Updatable();
		Updatable( Updatable && p_u ) noexcept = default;

		Util::EventHub::ScopedConnection * addUpdateFunction( const UpdateFunction & );
		void							   removeUpdateFunction( const Util::EventHub::ScopedConnection * const );

		inline bool isEmpty() const { return _connections.empty(); }

	  private:
		std::vector<std::unique_ptr<Util::EventHub::ScopedConnection>> _connections;
	};
} // namespace VTX::App::Component::Scene
#endif
