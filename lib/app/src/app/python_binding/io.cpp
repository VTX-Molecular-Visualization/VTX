#include "app/python_binding/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/io.hpp"
#include <optional>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <string>
#include <util/image.hpp>
#include <util/types.hpp>

namespace VTX::App::PythonBinding
{
	void IOBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Util::Image::E_FORMAT>( module, "IMAGE_FORMAT" );
		p_vtxModule.bindAction<
			App::Action::IO::Snapshot,
			const std::optional<FilePath>,
			const std::optional<FilePath>,
			Util::Image::E_FORMAT,
			const std::optional<size_t>,
			const std::optional<size_t>,
			const std::optional<float>>(
			"makeSnapshot",
			"Take a snapshot with optional output path, filename, dimensions and background opacity.",
			VTX::PythonBinding::Wrapper::VArg<std::optional<FilePath>>( "path", std::optional<FilePath>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<FilePath>>( "filename", std::optional<FilePath>() ),
			VTX::PythonBinding::Wrapper::VArg<Util::Image::E_FORMAT>( "format", Util::Image::E_FORMAT::PNG ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<size_t>>( "width", std::optional<size_t>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<size_t>>( "height", std::optional<size_t>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<float>>( "backgroundOpacity", std::optional<float>() )
		);

		p_vtxModule.bindAction<App::Action::IO::LoadSystem, const std::string &>(
			"openFile", "Open files at given path.", VTX::PythonBinding::Wrapper::Arg( "path" )
		);
		p_vtxModule.bindAction<App::Action::IO::AssociateTrajectory, const std::string &, const Entity &>(
			"associateTrajectory",
			"Associate input file trajectory to a system.",
			VTX::PythonBinding::Wrapper::Arg( "path" ),
			VTX::PythonBinding::Wrapper::Arg( "systemId" )
		);
		p_vtxModule.bindAction<App::Action::IO::DownloadSystem, const std::string &>(
			"download", "Retrieve a system from the RCSB PDB.", VTX::PythonBinding::Wrapper::Arg( "system_id" )
		);
	}
} // namespace VTX::App::PythonBinding
