#include "app/action/animation.hpp"
#include "app/core/animation/animation_system.hpp"
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <util/collection.hpp>

namespace VTX::App::Action::Animation
{
	Orient::Orient( const Util::Math::AABB & p_targetAABB ) :
		_orientInfo( App::SCENE().getCamera(), ORIENT_DURATION, p_targetAABB )
	{
	}
	Orient::Orient( const Vec3f & p_finalPosition, const Quatf & p_finalRotation, const Vec3f & p_targetPosition ) :
		_orientInfo( App::SCENE().getCamera(), ORIENT_DURATION, p_finalPosition, p_finalRotation, p_targetPosition )
	{
	}

	void Orient::execute() { ANIMATION_SYSTEM().launchAnimation<App::Core::Animation::Orient>( _orientInfo ); }

	ResetCamera::ResetCamera() : _resetInfo( App::SCENE().getCamera(), 0.f ) {}
	void ResetCamera::execute() { ANIMATION_SYSTEM().launchAnimation<App::Core::Animation::ResetCamera>( _resetInfo ); }
} // namespace VTX::App::Action::Animation
