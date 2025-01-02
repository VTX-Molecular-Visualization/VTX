#ifndef __VTX_APP_COMPONENT_SCENE_AABB__
#define __VTX_APP_COMPONENT_SCENE_AABB__

#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include <functional>
#include <util/math/aabb.hpp>

namespace VTX::App::Component::Scene
{
	class AABB : public Core::ECS::BaseComponent
	{
	  public:
		AABB() = default;

		void init();

		void setAABBComputationFunction( std::function<Util::Math::AABB()> p_recomputeAABB )
		{
			_recomputeAABB = p_recomputeAABB;
		}

		const Util::Math::AABB & getLocalAABB();
		const Util::Math::AABB & getWorldAABB();

		void setLocalAABB( const Util::Math::AABB & p_aabb ) { _aabb = p_aabb; };

		void invalidateAABB();

	  private:
		Util::Math::AABB _aabb;
		Util::Math::AABB _worldAabb;

		std::function<Util::Math::AABB()> _recomputeAABB;

		Component::Scene::Transform * _linkedTransform = nullptr;
	};
} // namespace VTX::App::Component::Scene
#endif
