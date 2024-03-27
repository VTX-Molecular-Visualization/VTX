#include <fmt/format.h>
#include <fstream>
#include <tools/mdprep/gromacs/pack.hpp>
#include <tools/mdprep/gromacs/util.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{

		ErrorReport generateMdpFile(
			const fs::path &	p_template,
			const fs::path &	p_dest,
			const std::string & p_replaceText
		) noexcept
		{
			fs::path templateFile = executableDirectory() / defaultGmxTemplatesRelativePath() / "nvt.mdp";
			if ( fs::exists( p_template ) )
			{
				std::string templateFileContent = getFileContent( p_template );

				replace( templateFileContent, "${nsteps}", p_replaceText );

				std::ofstream( p_dest ) << templateFileContent;
				return {};
			}
			return { false, fmt::format( "Template file <{}> not found.", templateFile.string() ) };
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
			std::to_string( p_in.nvtNsteps )
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
			std::to_string( p_in.nptNsteps )
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
			std::to_string( p_in.prodNsteps )
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
		return { false, fmt::format( "Template file <{}> not found.", templateFile.string() ) };
	}

	ErrorReport pack( const fs::path & p_dest, const CumulativeOuputFiles &, MdInstructions & ) noexcept { return {}; }

} // namespace VTX::Tool::Mdprep::Gromacs
