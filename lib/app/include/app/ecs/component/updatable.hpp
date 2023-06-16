#ifndef __VTX_APP_ECS_COMPONENT_UPDATABLE__
#define __VTX_APP_ECS_COMPONENT_UPDATABLE__

#include "app/application/scene_utility.hpp"
#include "app/ecs/core/base_component.hpp"
#include <functional>
#include <map>
#include <string>

namespace VTX::App::ECS::Component
{
	class Updatable : public ECS::Core::BaseComponent
	{
	  public:
		Updatable() = default;

		void addCallback( const std::string & p_key, const SceneUtility::UpdateCallback & p_callback )
		{
			_callbacks.emplace( p_key, p_callback );
		}
		void removeCallback( const std::string & p_key ) { _callbacks.erase( p_key ); }

		bool isEmpty() const { return _callbacks.empty(); }

		void update( const float p_deltaTime ) const
		{
			for ( const auto & pairKeyCallback : _callbacks )
			{
				pairKeyCallback.second( p_deltaTime );
			}
		}

	  private:
		std::map<std::string, SceneUtility::UpdateCallback> _callbacks
			= std::map<std::string, SceneUtility::UpdateCallback>();
	};
} // namespace VTX::App::ECS::Component
#endif
