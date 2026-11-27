#include "app/python_binding/camera.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/generic/name.hpp"
#include "app/helper/aabb.hpp"
#include "app/python_binding/identity.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include "app/python_binding/topology/types.hpp"
#include "app/scene/viewpoint.hpp"
#include <algorithm>
#include <functional>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/camera.hpp>
#include <stdexcept>
#include <string>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/string.hpp>
#include <utility>
#include <vector>

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

		struct Viewpoint
		{
			Entity entity;
		};

		void _validateViewpoint( const Entity p_entity )
		{
			if ( not REG().valid( p_entity )
				 || not REG().all_of<Scene::ViewPoint, Util::Math::Transform, Generic::Name>( p_entity ) )
			{
				throw pybind11::value_error( "Invalid viewpoint." );
			}
		}

		const Util::Math::Transform & _getViewpointTransform( const Viewpoint & p_viewpoint )
		{
			_validateViewpoint( p_viewpoint.entity );
			return REG().get<Util::Math::Transform>( p_viewpoint.entity );
		}

		std::vector<Viewpoint> _getViewpoints()
		{
			std::vector<Viewpoint> viewpoints;
			for ( const Entity entity : REG().view<Scene::ViewPoint, Util::Math::Transform, Generic::Name>() )
			{
				viewpoints.push_back( { entity } );
			}
			return viewpoints;
		}

		Viewpoint _getViewpoint( const std::string & p_name )
		{
			for ( const Viewpoint & viewpoint : _getViewpoints() )
			{
				if ( Util::String::equalsIgnoreCase( REG().get<Generic::Name>( viewpoint.entity ).name, p_name ) )
				{
					return viewpoint;
				}
			}
			throw pybind11::value_error( "No viewpoint named " + p_name + "." );
		}

		Util::Math::AABB _getAABB(
			const Entity							  p_entity,
			const std::optional<Topology::SystemItem> p_item,
			const Topology::RangeList &				  p_ranges
		)
		{
			return synchronizeCopy(
				[ & ]
				{
					Topology::getTopology( p_entity );
					return p_item ? Helper::AABB::get( p_entity, *p_item, p_ranges )
								  : Helper::AABB::getBonds( p_entity, p_ranges );
				}
			);
		}

		template<typename Action, typename... Args>
		void _animateCamera( const float p_duration, Args &&... p_args )
		{ executeAction<Action>( std::forward<Args>( p_args )..., p_duration ); }

		void _orient( const Util::Math::AABB & p_target, const float p_duration )
		{ _animateCamera<App::Action::Camera::Orient>( p_duration, p_target ); }

		template<typename T>
		void _defOrient( pybind11::class_<CameraView> & p_class, const std::optional<Topology::SystemItem> p_item )
		{
			p_class.def(
				"orient",
				[ p_item ]( const CameraView &, const T & p_target, const float p_duration )
				{
					if constexpr ( requires { p_target.ranges; } )
					{
						_orient( _getAABB( p_target.entity, p_item, p_target.ranges ), p_duration );
					}
					else
					{
						_orient(
							_getAABB( p_target.entity, p_item, Topology::RangeList( p_target.index ) ), p_duration
						);
					}
				},
				"Frame the target in the view.",
				pybind11::arg( "target" ),
				pybind11::arg( "duration" ) = 0.f
			);
		}

		template<typename Action, typename... Args>
		void _executeViewpointAction( const Viewpoint & p_viewpoint, Args &&... p_args )
		{
			executeOnMainThread(
				[ & ]()
				{
					_validateViewpoint( p_viewpoint.entity );
					Action action;
					Util::ActionManager::run( action, p_viewpoint.entity, std::forward<Args>( p_args )... );
				}
			);
		}

		struct SaveViewpointAction
		{
			void execute( Entity & p_created )
			{
				const std::vector<Viewpoint> previous = _getViewpoints();
				App::Action::Camera::SaveViewpoint().execute();
				for ( const Viewpoint & viewpoint : _getViewpoints() )
				{
					const bool isNew = std::ranges::none_of(
						previous,
						[ &viewpoint ]( const Viewpoint & p_previous ) { return p_previous.entity == viewpoint.entity; }
					);
					if ( isNew )
					{
						p_created = viewpoint.entity;
						return;
					}
				}
			}
		};

		Viewpoint _saveViewpoint( const std::optional<std::string> & p_name )
		{
			Entity created = InvalidEntity;
			executeAction<SaveViewpointAction>( std::ref( created ) );
			if ( created == InvalidEntity )
			{
				throw std::runtime_error( "Viewpoint creation failed." );
			}

			if ( p_name )
			{
				executeAction<App::Action::Camera::RenameViewPoint>( created, *p_name );
			}
			return { created };
		}
	} // namespace

	void CameraBinder::bind( Module & p_vtxModule )
	{
		const pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Renderer::PROJECTION>(
			module, "CAMERA_PROJECTION", "Camera projection mode."
		);

		bindIdentity(
			pybind11::class_<Viewpoint>( module, "Viewpoint", "Saved camera viewpoint.", pybind11::module_local() )
		)
			.def_property_readonly(
				"name",
				synchronized(
					[]( const Viewpoint & p_viewpoint )
					{
						_validateViewpoint( p_viewpoint.entity );
						return REG().get<Generic::Name>( p_viewpoint.entity ).name;
					}
				),
				"Viewpoint name."
			)
			.def_property_readonly(
				"position",
				synchronized( []( const Viewpoint & p_viewpoint )
							  { return _getViewpointTransform( p_viewpoint ).getPosition(); } ),
				"Camera position."
			)
			.def_property_readonly(
				"rotation",
				synchronized( []( const Viewpoint & p_viewpoint )
							  { return _getViewpointTransform( p_viewpoint ).getRotation(); } ),
				"Camera rotation quaternion."
			)
			.def_property_readonly(
				"target",
				synchronized(
					[]( const Viewpoint & p_viewpoint )
					{
						_validateViewpoint( p_viewpoint.entity );
						return REG().get<Scene::ViewPoint>( p_viewpoint.entity ).target;
					}
				),
				"Camera target point."
			)
			.def(
				"setName",
				[]( const Viewpoint & p_viewpoint, const std::string & p_name )
				{ _executeViewpointAction<App::Action::Camera::RenameViewPoint>( p_viewpoint, p_name ); },
				"Rename the viewpoint.",
				pybind11::arg( "name" )
			)
			.def(
				"setPosition",
				[]( const Viewpoint & p_viewpoint, const Vec3f & p_position )
				{ _executeViewpointAction<App::Action::Camera::SetViewPointPosition>( p_viewpoint, p_position ); },
				"Set the camera position.",
				pybind11::arg( "position" )
			)
			.def(
				"setRotation",
				[]( const Viewpoint & p_viewpoint, const Quatf & p_rotation )
				{ _executeViewpointAction<App::Action::Camera::SetViewPointRotation>( p_viewpoint, p_rotation ); },
				"Set the camera rotation quaternion.",
				pybind11::arg( "rotation" )
			)
			.def(
				"updateFromCamera",
				[]( const Viewpoint & p_viewpoint )
				{ _executeViewpointAction<App::Action::Camera::UpdateViewPointFromCamera>( p_viewpoint ); },
				"Store the current camera position, rotation and target in this viewpoint."
			)
			.def(
				"goTo",
				[]( const Viewpoint & p_viewpoint, const float p_duration )
				{
					_executeViewpointAction<App::Action::Camera::GoToViewPoint>( p_viewpoint, p_duration );
				},
				"Move the camera to this viewpoint.",
				pybind11::arg( "duration" ) = 0.f
			)
			.def(
				"delete",
				[]( const Viewpoint & p_viewpoint )
				{ _executeViewpointAction<App::Action::Camera::DeleteViewPoint>( p_viewpoint ); },
				"Delete this viewpoint."
			);

		auto cameraClass = pybind11::class_<CameraView>(
			module,
			"Camera",
			"Scene camera, available as vtx.camera. Durations are in milliseconds; a positive duration animates the "
			"camera and waits for the end.",
			pybind11::module_local()
		);
		cameraClass
			.def_property_readonly(
				"position",
				synchronized( []( const CameraView & ) { return _getCameraTransform().getPosition(); } ),
				"Position in world space."
			)
			.def_property_readonly(
				"rotation",
				synchronized( []( const CameraView & ) { return _getCameraTransform().getRotation(); } ),
				"Rotation quaternion."
			)
			.def_property_readonly(
				"fov",
				synchronized( []( const CameraView & ) { return _getCamera().fov; } ),
				"Vertical field of view in degrees."
			)
			.def_property_readonly(
				"nearClip",
				synchronized( []( const CameraView & ) { return _getCamera().near; } ),
				"Near clipping distance."
			)
			.def_property_readonly(
				"farClip",
				synchronized( []( const CameraView & ) { return _getCamera().far; } ),
				"Far clipping distance."
			)
			.def_property_readonly(
				"projection",
				synchronized( []( const CameraView & ) { return _getCamera().projection; } ),
				"Projection mode."
			)
			.def(
				"isPerspective",
				synchronized( []( const CameraView & )
							  { return _getCamera().projection == Renderer::PROJECTION::PERSPECTIVE; } ),
				"Whether the projection is perspective."
			)
			.def(
				"isOrthographic",
				synchronized( []( const CameraView & )
							  { return _getCamera().projection == Renderer::PROJECTION::ORTHOGRAPHIC; } ),
				"Whether the projection is orthographic."
			)
			.def(
				"setPosition",
				[]( const CameraView &, const Vec3f & p_position )
				{ executeAction<App::Action::Camera::SetPosition>( p_position ); },
				"Set the position in world space.",
				pybind11::arg( "position" )
			)
			.def(
				"setRotation",
				[]( const CameraView &, const Quatf & p_rotation )
				{ executeAction<App::Action::Camera::SetRotation>( p_rotation ); },
				"Set the rotation quaternion.",
				pybind11::arg( "rotation" )
			)
			.def(
				"lookAt",
				[]( const CameraView &, const Vec3f & p_target )
				{ executeAction<App::Action::Camera::LookAt>( p_target ); },
				"Rotate the camera to look at a point in world space.",
				pybind11::arg( "target" )
			)
			.def(
				"setFov",
				[]( const CameraView &, const float p_fov ) { executeAction<App::Action::Camera::SetFov>( p_fov ); },
				"Set the vertical field of view in degrees, clamped to [10, 90].",
				pybind11::arg( "fov" )
			)
			.def(
				"setNearClip",
				[]( const CameraView &, const float p_near )
				{ executeAction<App::Action::Camera::SetNearClip>( p_near ); },
				"Set the near clipping distance.",
				pybind11::arg( "near" )
			)
			.def(
				"setFarClip",
				[]( const CameraView &, const float p_far )
				{ executeAction<App::Action::Camera::SetFarClip>( p_far ); },
				"Set the far clipping distance.",
				pybind11::arg( "far" )
			)
			.def(
				"setProjectionOrthographic",
				[]( const CameraView & )
				{ executeAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>(); },
				"Use an orthographic projection."
			)
			.def(
				"setProjectionPerspective",
				[]( const CameraView & )
				{ executeAction<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>(); },
				"Use a perspective projection."
			)
			.def(
				"reset", []( const CameraView & ) { executeAction<App::Action::Camera::Reset>(); }, "Reset the camera."
			)
			.def(
				"orient",
				[]( const CameraView &, const float p_duration )
				{ _animateCamera<App::Action::Camera::Orient>( p_duration ); },
				"Frame the selection in the view, or the whole scene if nothing is selected.",
				pybind11::arg( "duration" ) = 0.f
			)
			.def(
				"orient",
				[]( const CameraView &, const Util::Math::AABB & p_target, const float p_duration )
				{ _orient( p_target, p_duration ); },
				"Frame a bounding box in the view.",
				pybind11::arg( "target" ),
				pybind11::arg( "duration" ) = 0.f
			)
			.def(
				"orient",
				[]( const CameraView &, const Topology::System & p_target, const float p_duration )
				{
					_orient(
						synchronizeCopy(
							[ & ]
							{
								Topology::getTopology( p_target.entity );
								return Helper::AABB::get( p_target.entity );
							}
						),
						p_duration
					);
				},
				"Frame a system in the view.",
				pybind11::arg( "target" ),
				pybind11::arg( "duration" ) = 0.f
			);

		_defOrient<Topology::Chain>( cameraClass, Topology::SystemItem::CHAIN );
		_defOrient<Topology::Residue>( cameraClass, Topology::SystemItem::RESIDUE );
		_defOrient<Topology::Atom>( cameraClass, Topology::SystemItem::ATOM );
		_defOrient<Topology::Bond>( cameraClass, std::nullopt );
		_defOrient<Topology::Category>( cameraClass, Topology::SystemItem::CATEGORY );
		_defOrient<Topology::ChainCollection>( cameraClass, Topology::SystemItem::CHAIN );
		_defOrient<Topology::ResidueCollection>( cameraClass, Topology::SystemItem::RESIDUE );
		_defOrient<Topology::AtomCollection>( cameraClass, Topology::SystemItem::ATOM );
		_defOrient<Topology::BondCollection>( cameraClass, std::nullopt );
		_defOrient<Topology::CategoryCollection>( cameraClass, Topology::SystemItem::CATEGORY );

		cameraClass
			.def(
				"straightTravel",
				[]( const CameraView &, const Vec3f & p_position, const Quatf & p_rotation, const float p_duration )
				{ _animateCamera<App::Action::Camera::StraightTravel>( p_duration, p_position, p_rotation ); },
				"Move the camera to a position and rotation.",
				pybind11::arg( "position" ),
				pybind11::arg( "rotation" ),
				pybind11::arg( "duration" ) = 0.f
			)
			.def(
				"saveViewpoint",
				[]( const CameraView &, const std::optional<std::string> & p_name )
				{ return _saveViewpoint( p_name ); },
				"Save the current camera as a new viewpoint and return it.",
				pybind11::arg( "name" ) = pybind11::none()
			)
			.def(
				"getViewpoints",
				synchronized( []( const CameraView & ) { return _getViewpoints(); } ),
				"Return all viewpoints."
			)
			.def(
				"getViewpoint",
				synchronized( []( const CameraView &, const std::string & p_name )
							  { return _getViewpoint( p_name ); } ),
				"Return a viewpoint by name, case insensitive. Raises ValueError if not found.",
				pybind11::arg( "name" )
			);
		module.attr( "camera" ) = CameraView {};
	}
} // namespace VTX::App::PythonBinding
