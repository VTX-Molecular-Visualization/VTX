#include <array>
#include <qprocess.h>
#include <re2/re2.h>
#include <tools/mdprep/gromacs/trjconv.hpp>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include "tools/mdprep/gromacs/job.hpp"

namespace VTX::Tool::Mdprep::Gromacs
{

	void prepareJob(
		const CumulativeOuputFiles & p_previousJobsOutputs,
		const fs::path &			 p_root,
		const std::string_view &	 p_folderName,
		TrjconvInstructions &		 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		auto lastGroFile = std::find_if(
			p_previousJobsOutputs.fileStringPtrs.begin(),
			p_previousJobsOutputs.fileStringPtrs.end(),
			[]( const std::string * p_ ) { return p_->ends_with( ".gro" ); }
		);
		if ( lastGroFile != std::end( p_previousJobsOutputs.fileStringPtrs ) )
			p_instructions.inputGro = *lastGroFile.operator*();
		p_instructions.outputGro = jobDir / ( p_instructions.fileStem + ".gro" );
	}

	constexpr char g_groupWaitingEnd[] = "Select a group: ";

	bool isWaitingForInput( const TrjconvInputs &, const std::string_view & p_stdout ) noexcept
	{
		return p_stdout.ends_with( g_groupWaitingEnd );
	}
	bool enterInput(
		TrjconvInputs & p_inputs,
		QProcess &		p_proc,
		std::string &	p_stdout,
		std::string &	p_stderr
	) noexcept
	{
		/*
		Some weird stuff is going on. Here is what we expect :

			Select group for centering
			Group     0 (         System) has 22485 elements
			Group     1 (        Protein) has  1230 elements
			Group     2 (      Protein-H) has   602 elements
			Group     3 (        C-alpha) has    76 elements
			Group     4 (       Backbone) has   228 elements
			Group     5 (      MainChain) has   305 elements
			Group     6 (   MainChain+Cb) has   375 elements
			Group     7 (    MainChain+H) has   380 elements
			Group     8 (      SideChain) has   850 elements
			Group     9 (    SideChain-H) has   297 elements
			Group    10 (    Prot-Masses) has  1230 elements
			Group    11 (    non-Protein) has 21255 elements
			Group    12 (          Water) has 21255 elements
			Group    13 (            SOL) has 21255 elements
			Group    14 (      non-Water) has  1230 elements
			Select a group:

		But all we have from the standard output is :

			Group     0 (         System) has 22485 elements
			Group     1 (        Protein) has  1230 elements
			Group     2 (      Protein-H) has   602 elements
			Group     3 (        C-alpha) has    76 elements
			Group     4 (       Backbone) has   228 elements
			Group     5 (      MainChain) has   305 elements
			Group     6 (   MainChain+Cb) has   375 elements
			Group     7 (    MainChain+H) has   380 elements
			Group     8 (      SideChain) has   850 elements
			Group     9 (    SideChain-H) has   297 elements
			Group    10 (    Prot-Masses) has  1230 elements
			Group    11 (    non-Protein) has 21255 elements
			Group    12 (          Water) has 21255 elements
			Group    13 (            SOL) has 21255 elements
			Group    14 (      non-Water) has  1230 elements
			Select a group:

		So the reason the software ask for input is vanished (not in stderr nor in stdout ??? WTF ???)
		*/

		if ( p_inputs.centeringDone == false )
		{
			std::string groupList;
			const RE2	centeringPattern { "\r?\n\r?\n(Group(.|\r|\n)+?)Select a group: " };
			if ( RE2::PartialMatch( { p_stderr }, centeringPattern, &groupList ) == false )
				return false;

			const RE2	proteinGroupPattern { "Group +?(\\d+) *?\\( *Protein *\\)" };
			std::string groupNum;

			if ( RE2::PartialMatch( { groupList }, proteinGroupPattern, &groupNum ) == false )
				return false;
			groupNum += '\n';
			p_stderr += groupNum;
			p_proc.write( groupNum.data() );
			p_proc.waitForBytesWritten();
			p_inputs.centeringDone = true;
			return true;
		}

		const RE2	proteinGroupPattern { "Group +?(\\d+) *?\\( *System *\\)" };
		std::string groupNum;
		if ( RE2::PartialMatch( { p_stderr }, proteinGroupPattern, &groupNum ) == false )
			return false;
		groupNum += '\n';
		p_stderr += groupNum;
		p_proc.write( groupNum.data() );
		p_proc.waitForBytesWritten();

		return true;
	}

	void convert( const TrjconvInstructions & p_in, GromacsJobData & p_out ) noexcept
	{
		p_out.arguments.push_back( "trjconv" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-s" );
		p_out.arguments.push_back( p_in.inputGro.string() );
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( p_in.outputGro.string() );
		p_out.expectedOutputFilesIndexes.push_back( p_out.arguments.size() - 1 );
		p_out.arguments.push_back( "-center" );
		p_out.arguments.push_back( "-pbc" );
		p_out.arguments.push_back( "res" );
		p_out.arguments.push_back( "-ur" );
		p_out.arguments.push_back( "compact" );
		p_out.interactiveSettings = TrjconvInputs();
	}

} // namespace VTX::Tool::Mdprep::Gromacs
