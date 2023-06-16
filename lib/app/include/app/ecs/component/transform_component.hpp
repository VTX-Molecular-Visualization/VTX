#ifndef __VTX_APP_ECS_COMPONENT_TRANSFORM__
#define __VTX_APP_ECS_COMPONENT_TRANSFORM__

#include "app/ecs/core/base_component.hpp"
#include "app/old/internal/math/transform.hpp"

namespace VTX::App::ECS::Component
{
	class Transform : public ECS::Core::BaseComponent
	{
	  public:
		Transform() = default;

		const Old::Internal::Math::Transform & getTransform() const { return _transform; };
		Old::Internal::Math::Transform &	   getTransform() { return _transform; };
		void setTransform( Old::Internal::Math::Transform p_transform ) { _transform = p_transform; };

	  private:
		Old::Internal::Math::Transform _transform = Old::Internal::Math::Transform();
	};
} // namespace VTX::App::ECS::Component

#endif
