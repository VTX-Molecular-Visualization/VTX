#include "app/python_binding/io.hpp"
#include "app/action/io.hpp"
#include "app/helper/io.hpp"
#include <optional>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <string>
#include <util/image.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		void _openFile( const std::string & p_path ) { App::Action::IO::LoadSystem().execute( FilePath( p_path ) ); }

		void _download( const std::string & p_systemId, const bool p_force )
		{
			if ( not Helper::IO::isValidSystemId( p_systemId ) )
			{
				throw pybind11::value_error( "Invalid system id: " + p_systemId + "." );
			}

			App::Action::IO::DownloadSystem().execute( p_systemId, p_force );
		}
	} // namespace

	void IOBinder::bind( Module & p_vtxModule )
	{
		const pybind11::module_ & module = p_vtxModule.pyModule();

		VTX::PythonBinding::Helper::declareEnum<Util::Image::E_FORMAT>(
			module, "IMAGE_FORMAT", "Snapshot image format."
		);
		p_vtxModule.bindAction<
			App::Action::IO::Snapshot,
			const std::optional<FilePath>,
			const std::optional<FilePath>,
			Util::Image::E_FORMAT,
			const std::optional<size_t>,
			const std::optional<size_t>,
			const std::optional<float>>(
			"makeSnapshot",
			"Save a screenshot. Defaults to the snapshots folder, a timestamp name, PNG and the current resolution.",
			VTX::PythonBinding::Wrapper::VArg<std::optional<FilePath>>( "path", std::optional<FilePath>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<FilePath>>( "filename", std::optional<FilePath>() ),
			VTX::PythonBinding::Wrapper::VArg<Util::Image::E_FORMAT>( "format", Util::Image::E_FORMAT::PNG ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<size_t>>( "width", std::optional<size_t>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<size_t>>( "height", std::optional<size_t>() ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<float>>( "backgroundOpacity", std::optional<float>() )
		);

		p_vtxModule.def(
			"openFile",
			&_openFile,
			"Load a molecular system file and wait until it is loaded.",
			VTX::PythonBinding::Wrapper::Arg( "path" )
		);
		p_vtxModule.def(
			"download",
			&_download,
			"Load a system from the RCSB PDB and wait until it is loaded. The cached copy is used if it exists, "
			"unless force is true. Raises ValueError for an invalid identifier.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::VArg<bool>( "force", false )
		);
	}
} // namespace VTX::App::PythonBinding
