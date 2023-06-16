#include "app/ecs/component/aabb_component.hpp"
#include "app/ecs/component/transform_component.hpp"
#include "app/ecs/core/base_entity.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/old/internal/math/transform.hpp"

namespace VTX::App::ECS::Component
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
			ECS::Core::BaseEntity entity = MAIN_REGISTRY().getEntity( *this );

			Old::Internal::Math::Transform transform;

			if ( MAIN_REGISTRY().hasComponent<ECS::Component::Transform>( entity ) )
			{
				ECS::Component::Transform & transformComponent
					= MAIN_REGISTRY().getComponent<ECS::Component::Transform>( entity );
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
} // namespace VTX::App::ECS::Component
