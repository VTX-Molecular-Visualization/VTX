#include <tools/mdprep/gromacs/grompp.hpp>
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include "tools/mdprep/gromacs/job.hpp"
#include <tools/mdprep/gromacs/util.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const fs::path g_defaultMinimizationTemplateRelativePath
			= fs::path { "data" } / "tools" / "mdprep" / "gromacs" / "templates" / "minim.mdp";

		void replace( std::string & p_text, const char * p_pattern, const std::string & p_repl ) noexcept
		{
			p_text.replace( p_text.find( p_pattern ), strnlen_s( p_pattern, 0xff ), p_repl );
		}
	} // namespace
	// Return the position of gmx binary relative to the vtx executable folder
	const fs::path & defaultMinimizationTemplateRelativePath() noexcept
	{
		return g_defaultMinimizationTemplateRelativePath;
	}

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

		// grompp takes .top as strict input and doesn't touch it, I believe. So no need to copy it
		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".top" ) )
		{
			p_instructions.inputTop = *fileStrPtr;
		}
		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".gro" ) )
		{
			p_instructions.inputGro = *fileStrPtr;
		}
		p_instructions.outputMdp = jobDir / ( p_instructions.fileStem + ".mdp" );
		p_instructions.outputTop = jobDir / ( p_instructions.fileStem + ".top" );
		p_instructions.outputTpr = jobDir / ( p_instructions.fileStem + ".tpr" );

		if ( auto fileStrPtr = getFirstFileOfType( p_previousJobsOutputs, ".mdp" ) )
		{
			p_instructions.inputMdp = *fileStrPtr;
		}
		else
		{
			fs::path mdpTemplate = executableDirectory() / defaultMinimizationTemplateRelativePath();
			if ( fs::exists( mdpTemplate ) == false )
				return;

			std::string mdpContent = getFileContent( mdpTemplate );
			replace( mdpContent, "${emtol}", fmt::format( "{:0.01f}", p_instructions.min.emtol ) );
			replace( mdpContent, "${emstep}", fmt::format( "{:0.03f}", p_instructions.min.emstep ) );
			replace( mdpContent, "${nsteps}", fmt::format( "{}", p_instructions.min.nsteps ) );
			p_instructions.inputMdp = jobDir / ( p_instructions.fileStem + "_in.mdp" );
			writeIntoFile( p_instructions.inputMdp, mdpContent );
		}
	}
	void convert( const GromppInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "grompp" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.inputMdp.string() );
		p_out.arguments.push_back( "-c" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( p_in.inputTop.string() );
		p_out.arguments.push_back( "-po" );
		p_out.arguments.push_back( p_in.outputMdp.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-pp" );
		p_out.arguments.push_back( p_in.outputTop.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputTpr.string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-maxwarn" );
		p_out.arguments.push_back( "1" );
	}
} // namespace VTX::Tool::Mdprep::Gromacs
