#ifndef __VTX_APP_COMPONENT_SCENE_TRANSFORM__
#define __VTX_APP_COMPONENT_SCENE_TRANSFORM__

#include "app/application/system/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include <util/math/transform.hpp>

namespace VTX::App::Component::Scene
{
	class Transform : public Core::ECS::BaseComponent
	{
		inline static const Application::System::ECSSystem::ComponentStaticIDRegistration<Transform> registration {
			"Scene::TransformComponent"
		};

	  public:
		Transform() = default;

		const Util::Math::Transform & getTransform() const { return _transform; };
		Util::Math::Transform &		  getTransform() { return _transform; };
		void						  setTransform( Util::Math::Transform p_transform ) { _transform = p_transform; };

	  private:
		Util::Math::Transform _transform = Util::Math::Transform();
	};
} // namespace VTX::App::Component::Scene

#endif
