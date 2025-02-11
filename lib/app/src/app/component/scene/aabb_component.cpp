#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/helper/math.hpp"
#include "app/vtx_app.hpp"
#include <util/math/transform.hpp>

namespace VTX::App::Component::Scene
{
	void AABB::init()
	{
		if ( ECS_REGISTRY().hasComponent<Component::Scene::Transform>( *this ) )
		{
			Component::Scene::Transform & transformComponent
				= ECS_REGISTRY().getComponent<Component::Scene::Transform>( *this );

			_linkedTransform = &transformComponent;

			_linkedTransform->onTransform += [ this ]( const Util::Math::Transform & ) { _worldAabb.invalidate(); };
		}
	}

	void AABB::invalidateAABB() { _aabb.invalidate(); }

	const Util::Math::AABB & AABB::getLocalAABB()
	{
		if ( not _aabb.isValid() )
		{
			_aabb = _recomputeAABB();
		}
		return _aabb;
	};

	const Util::Math::AABB & AABB::getWorldAABB()
	{
		if ( not _worldAabb.isValid() )
		{
			Core::ECS::BaseEntity entity = ECS_REGISTRY().getEntity( *this );

			if ( _linkedTransform != nullptr )
			{
				const Util::Math::Transform & transform = _linkedTransform->getTransform();
				_worldAabb = Helper::Math::applyTransformOnAABB( getLocalAABB(), transform );
			}
			else
			{
				_worldAabb = _aabb;
			}
		}

		return _worldAabb;
	};
} // namespace VTX::App::Component::Scene
