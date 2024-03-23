#include <tools/mdprep/gromacs/gromacs.hpp>

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{

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
			checkJobResults( currentJobData );
			if ( currentJobData.report.errorOccured )
				return false;
			for ( auto & fileStrPtrIdx : currentJobData.expectedOutputFilesIndexes )
			{
				p_in.outputs.fileStringPtrs.emplace(
					p_in.outputs.fileStringPtrs.begin(), &currentJobData.arguments.at( fileStrPtrIdx )
				);
			}
			return true;
		}
		std::array<const char *, g_NUM_PREPARATION_JOBS> g_jobNames {
			"1_pdb2gmx", "2_editconf", "3_solvate", "4_trjconv", "5_grompp", "6_genion", "7_grompp", "8_editconf",
		};
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
		if ( carryPreparationStep( p_in, p_in.grompp1, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.genion, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.grompp2, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
		if ( carryPreparationStep( p_in, p_in.editconf2, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
			return;
	}

	void createMdDirectory( const GromacsInstructions &, const fs::path & p_dest ) noexcept {}

} // namespace VTX::Tool::Mdprep::Gromacs
