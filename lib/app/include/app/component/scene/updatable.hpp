#ifndef __VTX_APP_COMPONENT_SCENE_UPDATABLE__
#define __VTX_APP_COMPONENT_SCENE_UPDATABLE__

#include "app/application/scene_utility.hpp"
#include "app/core/ecs/base_component.hpp"
#include <functional>
#include <map>
#include <string>

namespace VTX::App::Component::Scene
{
	class Updatable : public Core::ECS::BaseComponent
	{
	  public:
		Updatable() = default;

		void addCallback( const std::string & p_key, const Application::SceneUtility::UpdateCallback & p_callback )
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
		std::map<std::string, Application::SceneUtility::UpdateCallback> _callbacks;
	};
} // namespace VTX::App::Component::Scene
#endif
