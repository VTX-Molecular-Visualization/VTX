#include <tools/mdprep/gromacs/gromacs.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		std::array<const char *, g_NUM_PREPARATION_JOBS> g_jobNames {
			"1_pdb2gmx", "2_editconf", "3_solvate", "4_trjconv",  "5_grompp",
			"6_genion",	 "7_grompp",   "8_grompp",	"9_editconf",
		};

		void fillOutputsFromExpectations( GromacsInstructions & p_in, GromacsJobData & p_currentJobData )
		{
			for ( auto & fileStrPtrIdx : p_currentJobData.expectedOutputFilesIndexes )
			{
				p_in.outputs.fileStringPtrs.emplace(
					p_in.outputs.fileStringPtrs.begin(), &p_currentJobData.arguments.at( fileStrPtrIdx )
				);
			}
		}
		template<typename Instruction>
		void fillOutputs( GromacsInstructions & p_in, Instruction & p_stepIn, GromacsJobData & p_currentJobData )
		{
			fillOutputsFromExpectations( p_in, p_currentJobData );
		}
		template<>
		void fillOutputs( GromacsInstructions & p_in, GenionInstructions & p_stepIn, GromacsJobData & p_currentJobData )
		{
			fillOutputsFromExpectations( p_in, p_currentJobData );
			if ( auto fileStrPtr = getFirstFileOfType( p_in.outputs, ".top" ) )
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
		}
		template<>
		void fillOutputs(
			GromacsInstructions & p_in,
			Pdb2gmxInstructions & p_stepIn,
			GromacsJobData &	  p_currentJobData
		)
		{
			fillOutputsFromExpectations( p_in, p_currentJobData );
			if ( auto fileStrPtr = getFirstFileOfType( p_in.outputs, ".top" ) )
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
		}
		template<>
		void fillOutputs(
			GromacsInstructions & p_in,
			SolvateInstructions & p_stepIn,
			GromacsJobData &	  p_currentJobData
		)
		{
			fillOutputsFromExpectations( p_in, p_currentJobData );
			if ( auto fileStrPtr = getFirstFileOfType( p_in.outputs, ".top" ) )
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
		}

		template<typename Instruction>
		bool carryPreparationStep(
			GromacsInstructions & p_in,
			Instruction &		  p_stepIn,
			const char *		  p_stepName,
			int &				  stepNum
		) noexcept
		{
			p_stepIn.fileStem = p_in.fileStem;
			prepareJob( p_in.outputs, p_in.rootDir, p_stepName, p_stepIn );
			auto & currentJobData = p_in.jobData[ stepNum ];
			stepNum++;
			convert( p_stepIn, currentJobData );
			fs::path gmxExe = executableDirectory() / defaultGmxBinaryRelativePath();
			submitGromacsJob( gmxExe, currentJobData );
			currentJobData.postJobRoutine( p_in.rootDir / p_stepName, currentJobData, p_in.outputs );
			checkJobResults( currentJobData );
			if ( currentJobData.report.errorOccured )
				return false;

			fillOutputs( p_in, p_stepIn, currentJobData );
			return true;
		}
	} // namespace

	void prepareStructure(
		std::stop_token &	  p_token,
		const fs::path &	  p_structurePdb,
		GromacsInstructions & p_in
	) noexcept
	{
		p_in.fileStem		  = p_structurePdb.stem().string();
		p_in.pdb2gmx.inputPdb = p_structurePdb;

		int jobIdx = 0;

		if ( carryPreparationStep( p_in, p_in.pdb2gmx, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.editconf1, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.solvate, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.trjconv, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		p_in.gromppIons.step = E_GROMPP_STEP::ions;
		if ( carryPreparationStep( p_in, p_in.gromppIons, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.genion, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		p_in.gromppPosres.step = E_GROMPP_STEP::posres;
		if ( carryPreparationStep( p_in, p_in.gromppPosres, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		p_in.gromppEm.step = E_GROMPP_STEP::em;
		if ( carryPreparationStep( p_in, p_in.gromppEm, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.editconf2, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
	}

	void createMdDirectory( const GromacsInstructions &, const fs::path & p_dest ) noexcept {}

} // namespace VTX::Tool::Mdprep::Gromacs
