#include "app/python_binding/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/io.hpp"
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
