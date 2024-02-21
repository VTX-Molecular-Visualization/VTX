#ifndef __VTX_APP_COMPONENT_SCENE_AABB__
#define __VTX_APP_COMPONENT_SCENE_AABB__

#include "app/application/system/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include <functional>
#include <util/math/aabb.hpp>

namespace VTX::App::Component::Scene
{
	class AABB : public Core::ECS::BaseComponent
	{
	  private:
		inline static Application::System::ECSSystem::ComponentStaticIDRegistration<AABB> registration {
			"Scene::AABBComponent"
		};

	  public:
		AABB() = default;

		void setAABBComputationFunction( std::function<Util::Math::AABB()> p_recomputeAABB )
		{
			_recomputeAABB = p_recomputeAABB;
		}

		const Util::Math::AABB & getLocalAABB() const;
		const Util::Math::AABB & getWorldAABB() const;

		void setLocalAABB( const Util::Math::AABB & p_aabb ) { _aabb = p_aabb; };

	  private:
		mutable Util::Math::AABB		  _aabb		 = Util::Math::AABB();
		mutable Util::Math::AABB		  _worldAabb = Util::Math::AABB();
		std::function<Util::Math::AABB()> _recomputeAABB;
	};
} // namespace VTX::App::Component::Scene
#endif
