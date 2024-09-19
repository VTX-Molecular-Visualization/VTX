#include "app/action/animation.hpp"
#include "app/core/animation/animation_system.hpp"
#include "app/core/animation/orient.hpp"
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <util/collection.hpp>

namespace VTX::App::Action::Animation
{
	Orient::Orient( const Util::Math::AABB & p_targetAABB ) :
		_orientInfo( App::SCENE().getCamera(), Core::Animation::OrientInfo::ORIENT_DURATION, p_targetAABB )
	{
	}
	Orient::Orient( const Vec3f & p_finalPosition, const Quatf & p_finalRotation, const Vec3f & p_targetPosition ) :
		_orientInfo(
			App::SCENE().getCamera(),
			Core::Animation::OrientInfo::ORIENT_DURATION,
			p_finalPosition,
			p_finalRotation,
			p_targetPosition
		)
	{
	}

	void Orient::execute() { ANIMATION_SYSTEM().launchAnimation<App::Core::Animation::Orient>( _orientInfo ); }

	/*
	ResetCamera::ResetCamera() {}
	void ResetCamera::execute()
	{
		auto & camera	= App::SCENE().getCamera();
		Vec3f  distance = Core::Animation::Orient::computeCameraOrientPosition(
			 camera.getTransform().getFront(),
			 camera.getFov(),
			 App::SCENE().getAABB(),
			 Core::Animation::OrientInfo::ORIENT_ZOOM_FACTOR
		 );
		App::SCENE().getCamera().reset( distance );
	}
	*/

	ResetCamera::ResetCamera() : _resetInfo( App::SCENE().getCamera(), 0.f ) {}
	void ResetCamera::execute() { ANIMATION_SYSTEM().launchAnimation<App::Core::Animation::ResetCamera>( _resetInfo ); }
} // namespace VTX::App::Action::Animation
