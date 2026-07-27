#include "app/python_binding/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include <pybind11/pybind11.h>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <renderer/camera.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		struct CameraView
		{
		};

		const Renderer::Camera & _getCamera()
		{
			const Entity entity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
			return REG().get<Renderer::Camera>( entity );
		}

		const Util::Math::Transform & _getCameraTransform()
		{
			const Entity entity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
			return REG().get<Util::Math::Transform>( entity );
		}
	} // namespace

	void CameraBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Renderer::PROJECTION>( module, "CAMERA_PROJECTION" );

		pybind11::class_<CameraView>( module, "Camera", pybind11::module_local() )
			.def_property_readonly(
				"position", []( const CameraView & ) { return _getCameraTransform().getPosition(); }
			)
			.def_property_readonly(
				"rotation", []( const CameraView & ) { return _getCameraTransform().getRotation(); }
			)
			.def_property_readonly( "fov", []( const CameraView & ) { return _getCamera().fov; } )
			.def_property_readonly( "nearClip", []( const CameraView & ) { return _getCamera().near; } )
			.def_property_readonly( "farClip", []( const CameraView & ) { return _getCamera().far; } )
			.def_property_readonly( "projection", []( const CameraView & ) { return _getCamera().projection; } )
			.def(
				"isPerspective",
				[]( const CameraView & ) { return _getCamera().projection == Renderer::PROJECTION::PERSPECTIVE; }
			)
			.def(
				"isOrthographic",
				[]( const CameraView & ) { return _getCamera().projection == Renderer::PROJECTION::ORTHOGRAPHIC; }
			)
			.def(
				"setPosition",
				[]( const CameraView &, const Vec3f & p_position )
				{ executeAction<App::Action::Camera::SetPosition>( p_position ); },
				pybind11::arg( "position" )
			)
			.def(
				"setRotation",
				[]( const CameraView &, const Quatf & p_rotation )
				{ executeAction<App::Action::Camera::SetRotation>( p_rotation ); },
				pybind11::arg( "rotation" )
			)
			.def(
				"setFov",
				[]( const CameraView &, const float p_fov ) { executeAction<App::Action::Camera::SetFov>( p_fov ); },
				pybind11::arg( "fov" )
			)
			.def(
				"setNearClip",
				[]( const CameraView &, const float p_near )
				{ executeAction<App::Action::Camera::SetNearClip>( p_near ); },
				pybind11::arg( "near" )
			)
			.def(
				"setFarClip",
				[]( const CameraView &, const float p_far )
				{ executeAction<App::Action::Camera::SetFarClip>( p_far ); },
				pybind11::arg( "far" )
			)
			.def(
				"setProjectionOrthographic",
				[]( const CameraView & )
				{ executeAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>(); }
			)
			.def(
				"setProjectionPerspective",
				[]( const CameraView & )
				{ executeAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>(); }
			)
			.def( "reset", []( const CameraView & ) { executeAction<App::Action::Camera::Reset>(); } )
			.def( "orient", []( const CameraView & ) { executeAction<App::Action::Camera::Orient>(); } )
			.def(
				"straightTravel",
				[]( const CameraView &, const Vec3f & p_position, const Quatf & p_rotation, const float p_duration )
				{ executeAction<App::Action::Camera::StraightTravel>( p_position, p_rotation, p_duration ); },
				pybind11::arg( "position" ),
				pybind11::arg( "rotation" ),
				pybind11::arg( "duration" )
			);
		module.attr( "camera" ) = CameraView {};

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
			[]() { return _getCameraTransform().getPosition(); },
			"Return current camera position vector"
		);
		p_vtxModule.def(
			"getCameraRotation",
			[]() { return _getCameraTransform().getRotation(); },
			"Return current camera rotation vector"
		);
	}
} // namespace VTX::App::PythonBinding
