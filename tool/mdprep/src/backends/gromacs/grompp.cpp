#include <tool/mdprep/backends/gromacs/grompp.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const fs::path &			 p_root,
		const std::string_view &	 p_folderName,
		GromppInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".gro" ) )
		{
			p_instructions.inputGro = *fileStrPtr;
		}
		p_instructions.outputMdp = jobDir / ( p_instructions.fileStem + ".mdp" );
		p_instructions.outputTop = jobDir / ( p_instructions.fileStem + ".top" );
		p_instructions.outputTpr = jobDir / ( p_instructions.fileStem + ".tpr" );

		switch ( p_instructions.step )
		{
		case E_GROMPP_STEP::em:
		{
			fs::path	mdpTemplate = executableDirectory() / defaultGmxTemplatesRelativePath() / "minim.mdp";
			std::string mdpContent	= getFileContent( mdpTemplate );
			replace( mdpContent, "${emtol}", fmt::format( "{:0.01f}", p_instructions.min.emtol ) );
			replace( mdpContent, "${emstep}", fmt::format( "{:0.03f}", p_instructions.min.emstep ) );
			replace( mdpContent, "${nsteps}", fmt::format( "{}", p_instructions.min.nsteps ) );
			p_instructions.inputMdp = jobDir / ( p_instructions.fileStem + "_in.mdp" );
			writeIntoFile( p_instructions.inputMdp, mdpContent );
			break;
		}
		case E_GROMPP_STEP::ions:
		{
			fs::path mdpTemplate = executableDirectory() / defaultGmxTemplatesRelativePath() / "ions.mdp";
			if ( fs::exists( mdpTemplate ) == false )
				return;

			p_instructions.inputMdp = jobDir / ( p_instructions.fileStem + "_in.mdp" );
			fs::copy_file( mdpTemplate, p_instructions.inputMdp );
			break;
		}
		case E_GROMPP_STEP::posres:
		{
			fs::path mdpTemplate = executableDirectory() / defaultGmxTemplatesRelativePath() / "posres.mdp";
			if ( fs::exists( mdpTemplate ) == false )
				return;

			p_instructions.inputMdp = jobDir / ( p_instructions.fileStem + "_in.mdp" );
			fs::copy_file( mdpTemplate, p_instructions.inputMdp );
			break;
		}
		default: break;
		}
		// grompp takes .top as strict input and doesn't touch it, I believe. So no need to copy it
		p_instructions.inputTop = p_previousJobsOutputs.lastUncompiledTop;
	}
	void convert( const GromppInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "grompp" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.inputMdp.string() );
		p_out.arguments.push_back( "-c" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-r" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( p_in.inputTop.string() );
		p_out.arguments.push_back( "-po" );
		p_out.arguments.push_back( p_in.outputMdp.string() );
		if ( p_in.step == E_GROMPP_STEP::em ) // When in posres mode, we don't want anything else than the .top file
			setLastArgumentAsExpectedOutputFile( p_out );
		if ( p_in.step == E_GROMPP_STEP::em || p_in.step == E_GROMPP_STEP::posres )
		{
			p_out.arguments.push_back( "-pp" );
			p_out.arguments.push_back( p_in.outputTop.string() );
			setLastArgumentAsExpectedOutputFile( p_out );
		}
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputTpr.string() );
		if ( p_in.step != E_GROMPP_STEP::posres ) // When in posres mode, we don't want anything else than the .top file
			setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-maxwarn" );
		p_out.arguments.push_back( "1" );
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
