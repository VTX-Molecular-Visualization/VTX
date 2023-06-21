#include "app/component/scene/aabb_component.hpp"
#include "app/application/registry_manager.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/old/internal/math/transform.hpp"

namespace VTX::App::Component::Scene
{
	const Util::Math::AABB & AABB::getLocalAABB() const
	{
		if ( _aabb.isValid() )
			_aabb = _recomputeAABB();
		return _aabb;
	};

	const Util::Math::AABB & AABB::getWorldAABB() const
	{
		if ( !_worldAabb.isValid() )
		{
			Core::ECS::BaseEntity entity = Application::MAIN_REGISTRY().getEntity( *this );

			Old::Internal::Math::Transform transform;

			if ( Application::MAIN_REGISTRY().hasComponent<Component::Scene::Transform>( entity ) )
			{
				Component::Scene::Transform & transformComponent
					= Application::MAIN_REGISTRY().getComponent<Component::Scene::Transform>( entity );
				const Old::Internal::Math::Transform & transform = transformComponent.getTransform();

				std::vector<Vec3f> aabbSummits = getLocalAABB().getSummits();

				_worldAabb = Util::Math::AABB();
				for ( const Vec3f & summit : aabbSummits )
				{
					const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
					_worldAabb.extend( worldSummit );
				}
			}
			else
			{
				_worldAabb = _aabb;
			}
		}

		return _worldAabb;
	};
} // namespace VTX::App::Component::Scene
