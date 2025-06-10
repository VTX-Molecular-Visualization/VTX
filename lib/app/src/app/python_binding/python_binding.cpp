
#include "app/python_binding/python_binding.hpp"
#include "app/action/application.hpp"
#include "app/action/camera.hpp"
#include "app/action/io.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/python_binding/selection/binder.hpp"
#include "app/python_binding/viewpoint_manager.hpp"
#include <python_binding/action.hpp>
#include <python_binding/binder.hpp>
#include <python_binding/binding/helper.hpp>
#include <python_binding/binding/vtx_module.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <util/logger.hpp>

namespace VTX::App::PythonBinding
{
	void VTXAppBinder::bind( VTX::PythonBinding::PyTXModule & p_vtxmodule )
	{
		VTX::VTX_INFO( "Applying binding on module." );

		VTX::PythonBinding::Wrapper::Module commands = p_vtxmodule.commands();

		pybind11::module_ * apiModulePtr = nullptr;
		p_vtxmodule.api().getPythonModule( &apiModulePtr );

		commands.bindAction<App::Action::Application::NewScene>( "newScene", "Clear scene." );

		pybind11::module_ * commandModulePtr = nullptr;
		p_vtxmodule.commands().getPythonModule( &commandModulePtr );
		Selection::bind_selection( *commandModulePtr );
		VTX::PythonBinding::Helper::declareEnum<Util::Image::E_FORMAT>( *commandModulePtr, "IMAGE_FORMAT" );
		commands.bindAction<
			App::Action::Io::Snapshot,
			const std::string &,
			Util::Image::E_FORMAT,
			const size_t &,
			const size_t &>(
			"makeSnapshot",
			"Take a snapshot and generate a file at [arg1] location of [arg2] format with dimension of [arg3]x[arg4].",
			VTX::PythonBinding::Wrapper::Arg( "path" ),
			VTX::PythonBinding::Wrapper::Arg( "format" ),
			VTX::PythonBinding::Wrapper::Arg( "width" ),
			VTX::PythonBinding::Wrapper::Arg( "height" )
		);
		commands.bindAction<App::Action::Io::Open, const std::string &>(
			"openFile", "Open files at given path.", VTX::PythonBinding::Wrapper::Arg( "path" )
		);
		commands.bindAction<App::Action::Io::OpenScene, const std::string &>(
			"openScene", "Open scene at given path.", VTX::PythonBinding::Wrapper::Arg( "path" )
		);
		commands.bindAction<App::Action::Io::DownloadSystem, const char *>(
			"download", "Retrieve a system from the RCSB PDB.", VTX::PythonBinding::Wrapper::Arg( "system_id" )
		);
		commands.bindAction<App::Action::Io::SaveScene, const std::string &>(
			"save", "Save scene.", VTX::PythonBinding::Wrapper::VArg<std::string>( "path", "" )
		);
		commands.bindAction<App::Action::Io::SaveSettings>( "saveSettings", "Save settings." );
		commands.bindAction<App::Action::Io::ReloadSettings>( "reloadSettings", "Reload settings." );
		commands.bindAction<App::Action::Io::ResetSettings>( "resetSettings", "Reset settings." );

		commands.bindAction<App::Action::Scene::ClearScene>( "clear", "Clear scene." );

		commands.bindAction<App::Action::Camera::SetCameraProjectionOrthographic>(
			"setCameraProjectionOrthographic", "Set the render projection into Orthographic mode."
		);
		commands.bindAction<App::Action::Camera::SetCameraProjectionPerspective>(
			"setCameraProjectionPerspective", "Set the render projection into Perspective mode."
		);
		commands.bindAction<App::Action::Camera::ToggleCameraProjection>(
			"toggleCameraProjection", "Toggle the render projection between Perspective and Orthographic mode."
		);
		commands.bindAction<App::Action::Camera::Reset>( "resetCamera", "Put the camera back in the initial space." );

		// TODO : test the stuff below after threading
		commands.bindAction<App::Action::Camera::MoveCamera, TravelViewpoint>(
			"travelTo",
			"Travel to a point in space and rotate the camera within the travel time window.",
			VTX::PythonBinding::Wrapper::Arg( "travelViewpoint" )
		);
		commands.bindAction<App::Action::Camera::MoveCamera, Vec3f, Quatf, float>(
			"travelTo",
			"Travel to a point in space and rotate the camera within the travel time window.",
			VTX::PythonBinding::Wrapper::Arg( "position" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation" ),
			VTX::PythonBinding::Wrapper::Arg( "travelTime" )
		);
		commands.bindAction<App::Action::Camera::MoveCamera, float, float, float, float, float, float, float, float>(
			"travelTo",
			"Travel to a point in space and rotate the camera within the travel time window.",
			VTX::PythonBinding::Wrapper::Arg( "position_x" ),
			VTX::PythonBinding::Wrapper::Arg( "position_y" ),
			VTX::PythonBinding::Wrapper::Arg( "position_z" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation_x" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation_y" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation_z" ),
			VTX::PythonBinding::Wrapper::Arg( "rotation_w" ),
			VTX::PythonBinding::Wrapper::Arg( "travelTime" )
		);
		commands.def(
			"getCameraPosition",
			[]() { return SCENE().getCamera().getTransform().getPosition(); },
			"Return current camera position"
		);
		commands.def(
			"getCameraRotation",
			[]() { return SCENE().getCamera().getTransform().getRotation(); },
			"Return current camera position"
		);
		pybind11::class_<TravelViewpointManager>( *commandModulePtr, "TravelViewpointManager" )
			.def( pybind11::init<>() )
			.def<void ( TravelViewpointManager::* )()>( "registerCurrent", &TravelViewpointManager::registerCurrent )
			.def<void ( TravelViewpointManager::* )( float )>(
				"registerCurrent", &TravelViewpointManager::registerCurrent
			)
			.def( "setDefaultTravelTime", &TravelViewpointManager::setDefaultTravelTime )
			.def(
				"__getitem__",
				[]( TravelViewpointManager & _, const size_t & idx )
				{
					if ( _.size() > idx )
						return _[ idx ];
					else
						throw pybind11::index_error(
							"Provided index is greater than or equal to the size of the list."
						);
				}
			)
			.def( "__len__", &TravelViewpointManager::size )
			.def(
				"__iter__",
				[]( TravelViewpointManager & c ) { return pybind11::make_iterator( c.begin(), c.end() ); },
				pybind11::keep_alive<0, 1>()
			);
		pybind11::class_<TravelViewpoint>( *commandModulePtr, "TravelViewpoint" )
			.def( pybind11::init<>() )
			.def_readwrite( "position", &TravelViewpoint::position )
			.def_readwrite( "rotation", &TravelViewpoint::rotation )
			.def_readwrite( "travelTime", &TravelViewpoint::travelTime );
	}

	void VTXAppBinder::importHeaders()
	{
		VTX::PythonBinding::importObject(
			fmt::format( "{}.Command", VTX::PythonBinding::vtx_module_name() ), "select"
		);
		VTX::PythonBinding::importObject(
			fmt::format( "{}.Command", VTX::PythonBinding::vtx_module_name() ), "intersect"
		);
		VTX::PythonBinding::importObject(
			fmt::format( "{}.Command", VTX::PythonBinding::vtx_module_name() ), "exclusive"
		);
	}

} // namespace VTX::App::PythonBinding
