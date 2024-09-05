#include <fmt/format.h>
#include <fstream>
#include <tool/mdprep/backends/gromacs/pack.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>
#include <vector>

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	namespace
	{
		std::vector<std::pair<const char *, std::string>> generateKeyValues( const MdInstructions::Equil & p_ ) noexcept
		{
			return {
				{ "${nsteps}", std::to_string( p_.nsteps ) },		{ "${dt}", std::to_string( p_.dt ) },
				{ "${nstxout}", std::to_string( p_.nstxout ) },		{ "${nstvout}", std::to_string( p_.nstvout ) },
				{ "${nstenergy}", std::to_string( p_.nstenergy ) }, { "${nstlog}", std::to_string( p_.nstlog ) },
			};
		}
		std::vector<std::pair<const char *, std::string>> generateKeyValues( const MdInstructions::Prod & p_ ) noexcept
		{
			return {
				{ "${nsteps}", std::to_string( p_.nsteps ) },
				{ "${dt}", std::to_string( p_.dt ) },
				{ "${nstxout}", std::to_string( p_.nstxout ) },
				{ "${nstvout}", std::to_string( p_.nstvout ) },
				{ "${nstenergy}", std::to_string( p_.nstenergy ) },
				{ "${nstlog}", std::to_string( p_.nstlog ) },
				{ "${nstxout-compressed}", std::to_string( p_.nstxout_compressed ) },
			};
		}
		ErrorReport generateMdpFile(
			const fs::path &										  p_template,
			const fs::path &										  p_dest,
			const std::vector<std::pair<const char *, std::string>> & p_replaceText
		) noexcept
		{
			fs::path templateFile = executableDirectory() / defaultGmxTemplatesRelativePath() / "nvt.mdp";
			if ( fs::exists( p_template ) )
			{
				std::string templateFileContent = getFileContent( p_template );

				for ( auto & pair : p_replaceText )
					replace( templateFileContent, pair.first, pair.second );
				std::ofstream( p_dest ) << templateFileContent;
				return {};
			}
			return { true, fmt::format( "Template file <{}> not found.", templateFile.string() ) };
		}
	} // namespace
	ErrorReport createNvtMdp( const fs::path & p_dest, MdInstructions & p_in ) noexcept
	{
		if ( fs::exists( p_dest ) == false )
		{
			fs::create_directories( p_dest );
		}
		return generateMdpFile(
			executableDirectory() / defaultGmxTemplatesRelativePath() / "nvt.mdp",
			p_dest / "nvt.mdp",
			generateKeyValues( p_in.nvt )
		);
	}

	ErrorReport createNptMdp( const fs::path & p_dest, MdInstructions & p_in ) noexcept
	{
		if ( fs::exists( p_dest ) == false )
		{
			fs::create_directories( p_dest );
		}
		return generateMdpFile(
			executableDirectory() / defaultGmxTemplatesRelativePath() / "npt.mdp",
			p_dest / "npt.mdp",
			generateKeyValues( p_in.npt )
		);
	}

	ErrorReport createProdMdp( const fs::path & p_dest, MdInstructions & p_in ) noexcept
	{
		if ( fs::exists( p_dest ) == false )
		{
			fs::create_directories( p_dest );
		}
		return generateMdpFile(
			executableDirectory() / defaultGmxTemplatesRelativePath() / "prod.mdp",
			p_dest / "prod.mdp",
			generateKeyValues( p_in.prod )
		);
	}

	ErrorReport createWorkflowPy( const fs::path & p_dest, MdInstructions & p_in ) noexcept
	{
		if ( fs::exists( p_dest ) == false )
		{
			fs::create_directories( p_dest );
		}
		fs::path templateFile = executableDirectory() / defaultGmxTemplatesRelativePath() / "runMD.py";
		if ( fs::exists( templateFile ) )
		{
			std::string templateFileContent = getFileContent( templateFile );

			replace( templateFileContent, "${fileStem}", p_in.fileStem );

			std::ofstream( p_dest / "runMD.py" ) << templateFileContent;
			return {};
		}
		return { true, fmt::format( "Template file <{}> not found.", templateFile.string() ) };
	}

	ErrorReport pack(
		const fs::path &			 p_dest,
		const CumulativeOuputFiles & p_prepareOutputs,
		MdInstructions &			 p_instructions
	) noexcept
	{
		ErrorReport err = createNvtMdp( p_dest, p_instructions );
		if ( err.error )
			return err;
		err = createNptMdp( p_dest, p_instructions );
		if ( err.error )
			return err;
		err = createProdMdp( p_dest, p_instructions );
		if ( err.error )
			return err;
		err = createWorkflowPy( p_dest, p_instructions );
		if ( err.error )
			return err;

		auto fileStrPtr = getFirstFileOfType( p_prepareOutputs, ".tpr" );
		if ( fileStrPtr == nullptr )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		fs::path file { *fileStrPtr };
		if ( fs::exists( file ) == false )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		fs::copy_file( file, p_dest / "em.tpr" );

		fileStrPtr = getFirstFileOfType( p_prepareOutputs, ".mdp" );
		if ( fileStrPtr == nullptr )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		file = *fileStrPtr;
		if ( fs::exists( file ) == false )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		fs::copy_file( file, p_dest / "em.mdp" );

		fileStrPtr = getFileOfType( p_prepareOutputs, 2, ".top" );
		if ( fileStrPtr == nullptr )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		file = *fileStrPtr;
		if ( fs::exists( file ) == false )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		fs::copy_file( file, p_dest / "posres.top" );

		fileStrPtr = getFileOfType( p_prepareOutputs, 1, ".top" );
		if ( fileStrPtr == nullptr )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		file = *fileStrPtr;
		if ( fs::exists( file ) == false )
			return { true, "Preparation failed. Please check preparation step in red to learn more." };

		fs::copy_file( file, p_dest / "topol.top" );

		return {};
	}

} // namespace VTX::Tool::Mdprep::backends::Gromacs
