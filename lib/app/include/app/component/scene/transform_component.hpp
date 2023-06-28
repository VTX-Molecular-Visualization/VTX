#ifndef __VTX_APP_COMPONENT_SCENE_TRANSFORM__
#define __VTX_APP_COMPONENT_SCENE_TRANSFORM__

#include "app/core/ecs/base_component.hpp"
#include "app/old/internal/math/transform.hpp"

namespace VTX::App::Component::Scene
{
	class Transform : public Core::ECS::BaseComponent
	{
	  public:
		Transform() = default;

		const Old::Internal::Math::Transform & getTransform() const { return _transform; };
		Old::Internal::Math::Transform &	   getTransform() { return _transform; };
		void setTransform( Old::Internal::Math::Transform p_transform ) { _transform = p_transform; };

	  private:
		Old::Internal::Math::Transform _transform = Old::Internal::Math::Transform();
	};
} // namespace VTX::App::Component::Scene

#endif
