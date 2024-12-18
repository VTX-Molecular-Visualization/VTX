#ifndef __VTX_APP_COMPONENT_SCENE_UPDATABLE__
#define __VTX_APP_COMPONENT_SCENE_UPDATABLE__

#include "app/core/ecs/base_component.hpp"
#include <functional>
#include <util/callback.hpp>

namespace VTX::App::Component::Scene
{
	using UpdateFunction = std::function<void( const float, const float )>;

	class Updatable : public Core::ECS::BaseComponent
	{
	  public:
		Updatable() = default;
		virtual ~Updatable();

		Util::CallbackId addUpdateFunction( const UpdateFunction & );
		void			 removeUpdateFunction( const Util::CallbackId );

		inline bool isEmpty() const { return _callbackIds.empty(); }

	  private:
		std::vector<Util::CallbackId> _callbackIds;
	};
} // namespace VTX::App::Component::Scene
#endif
