#include "ui/action/animation.hpp"
#include "ui/core/animation/animation_system.hpp"
#include "ui/internal/animation/orient.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/visualization.hpp"
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/core/collection.hpp>

namespace VTX::UI::Action::Animation
{
	Orient::Orient( const Util::Math::AABB & p_targetAABB ) :
		_orientInfo( App::SCENE().getCamera(), ORIENT_DURATION, p_targetAABB )
	{
	}
	Orient::Orient( const Vec3f & p_finalPosition, const Quatf & p_finalRotation, const Vec3f & p_targetPosition ) :
		_orientInfo( App::SCENE().getCamera(), ORIENT_DURATION, p_finalPosition, p_finalRotation, p_targetPosition )
	{
	}

	void Orient::execute()
	{
		UI::Internal::Animation::Orient orient = UI::Internal::Animation::Orient( _orientInfo );
		ANIMATION_SYSTEM().launchAnimation( orient );
	}

	void ResetCamera::execute()
	{
		// TODO real reset (see VTX 0.4.1)
		App::SCENE().getCamera().getTransform().setPosition( VEC3F_ZERO );
	}
} // namespace VTX::UI::Action::Animation
