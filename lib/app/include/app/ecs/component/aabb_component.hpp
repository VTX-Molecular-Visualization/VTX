#ifndef __VTX_APP_ECS_COMPONENT_AABB__
#define __VTX_APP_ECS_COMPONENT_AABB__

#include "app/ecs/core/base_component.hpp"
#include "app/old/internal/math/transform.hpp"
#include <functional>
#include <util/math/aabb.hpp>

namespace VTX::App::ECS::Component
{
	class AABB : public ECS::Core::BaseComponent
	{
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
} // namespace VTX::App::ECS::Component
#endif
