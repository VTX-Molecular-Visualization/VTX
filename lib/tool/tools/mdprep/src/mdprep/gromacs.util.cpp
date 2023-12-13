#include "tools/mdprep/gromacs.util.hpp"
#include <filesystem>
#include <format>
#include <optional>
#include <qapplication.h>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executable_directory;
	const fs::path &		executable_directory() noexcept
	{
		if ( !g_executable_directory.has_value() )
		{
			// int				 argc = 0;
			// QCoreApplication app( argc, nullptr );
			// QString			 qpath = app.applicationDirPath();
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );
			g_executable_directory.emplace( path_str );
		}
		return g_executable_directory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const char g_ff_suffix[] = ".ff";
	}

	const fs::path g_default_ff_directory_relative_path
		= fs::path( "data" ) / "tool" / "tools" / "mdprep" / "gromacs" / "top";
	const fs::path & default_ff_directory_relative_path() noexcept { return g_default_ff_directory_relative_path; }

	void declare_ff_directory( const std::filesystem::path & path ) noexcept
	{
		std::string	   path_str = path.string();
		QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
		qputenv( "GMXLIB", env_arg );
	}

	std::vector<forcefield> list_forcefields( const fs::path & data_dir )
	{
		if ( !fs::is_directory( data_dir ) )
		{
			auto err_str = std::format( "Directory <{}> not found", data_dir.string() );
			throw VTX::IOException( err_str );
		}
		std::vector<forcefield> out;

		for ( auto & fs_element : fs::directory_iterator( data_dir ) )
		{
			if ( !fs_element.is_directory() )
				continue;

			auto filename = fs_element.path().filename().string();
			if ( filename.ends_with( g_ff_suffix ) )
			{
				out.emplace_back( forcefield { .forcefield_folder_path = fs_element.path().string() } );
				continue;
			}
		}

		return out;
	}

	void convert( const pdb2gmx_instructions &, gromacs_command_args & ) noexcept {}
	void convert( const solvate_instructions &, gromacs_command_args & ) noexcept {}

	std::string_view forcefield::get_name() const
	{
		size_t filename_pos
			= this->forcefield_folder_path.find( fs::path( this->forcefield_folder_path ).filename().string() );
		size_t extension_pos = this->forcefield_folder_path.size() - ( sizeof( g_ff_suffix ) - 1 );
		return std::string_view { std::next( this->forcefield_folder_path.begin(), filename_pos ),
								  std::next( this->forcefield_folder_path.begin(), extension_pos ) };
	}
} // namespace VTX::Tool::Mdprep::Gromacs
