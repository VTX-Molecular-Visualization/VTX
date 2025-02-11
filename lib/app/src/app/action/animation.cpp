#include "app/action/animation.hpp"
#include "app/animation/orient.hpp"
#include <app/application/scene.hpp>
#include <app/component/controller.hpp>
#include <app/component/render/camera.hpp>
#include <util/collection.hpp>

namespace VTX::App::Action::Animation
{

	void Orient::execute()
	{
		Component::Controller & component = ECS_REGISTRY().getComponent<Component::Controller>( SCENE().getCamera() );
		component.launchAnimation<App::Animation::Orient>( App::SCENE().getCamera(), _target );
	}

} // namespace VTX::App::Action::Animation
