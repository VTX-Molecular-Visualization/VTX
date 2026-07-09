#include "app/python_binding/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include <python_binding/wrapper/arg.hpp>
#include <renderer/camera.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::PythonBinding
{
	void CameraBinder::bind( Module & p_vtxModule )
	{
		p_vtxModule.bindAction<App::Action::Camera::SetPosition, const Vec3f &>(
			"setCameraPosition", "Set camera position.", VTX::PythonBinding::Wrapper::Arg( "position" )
		);
		p_vtxModule.bindAction<App::Action::Camera::SetRotation, const Quatf &>(
			"setCameraRotation", "Set camera rotation.", VTX::PythonBinding::Wrapper::Arg( "rotation" )
		);
		p_vtxModule.bindAction<App::Action::Camera::SetFov, const float>(
			"setCameraFov", "Set camera field of view.", VTX::PythonBinding::Wrapper::Arg( "fov" )
		);
		p_vtxModule.bindAction<App::Action::Camera::SetNearClip, const float>(
			"setCameraNearClip", "Set camera near clipping plane.", VTX::PythonBinding::Wrapper::Arg( "near" )
		);
		p_vtxModule.bindAction<App::Action::Camera::SetFarClip, const float>(
			"setCameraFarClip", "Set camera far clipping plane.", VTX::PythonBinding::Wrapper::Arg( "far" )
		);
		p_vtxModule.bindAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>(
			"setCameraProjectionOrthographic", "Set the render projection into Orthographic mode."
		);
		p_vtxModule.bindAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>(
			"setCameraProjectionPerspective", "Set the render projection into Perspective mode."
		);
		p_vtxModule.bindAction<App::Action::Camera::Reset>(
			"resetCamera", "Put the camera back in the initial space."
		);
		p_vtxModule.bindAction<App::Action::Camera::Orient>(
			"orientCamera", "Orient camera to fit the current selection, or the scene if no selection is active."
		);
		p_vtxModule.bindAction<App::Action::Camera::StraightTravel, const Vec3f &, const Quatf &, const float>(
			"straightTravelCamera",
			"Travel camera in a straight line to target position and rotation.",
			VTX::PythonBinding::Wrapper::Arg( "position" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation" ),
			VTX::PythonBinding::Wrapper::Arg( "duration" )
		);
		p_vtxModule.def(
			"getCameraPosition",
			[]()
			{
				const auto [ ent, _, transform ]
					= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
				return transform.getPosition();
			},
			"Return current camera position vector"
		);
		p_vtxModule.def(
			"getCameraRotation",
			[]()
			{
				const auto [ ent, _, transform ]
					= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
				return transform.getRotation();
			},
			"Return current camera rotation vector"
		);
	}
} // namespace VTX::App::PythonBinding
