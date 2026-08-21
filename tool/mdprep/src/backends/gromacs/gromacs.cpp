#include <app/services.hpp>
#include <app/session.hpp>
#include <latch>
#include <thread>
#include <util/logger.hpp>
#include <util/thread/base_thread.hpp>
#include <util/thread/thread_manager.hpp>
//
#include <tool/mdprep/backends/gromacs/gromacs.hpp>
#include <tool/mdprep/gateway/shared.hpp>

namespace VTX::Tool::Mdprep::backends::Gromacs
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
		{ fillOutputsFromExpectations( p_in, p_currentJobData ); }

		template<>
		void fillOutputs( GromacsInstructions & p_in, GenionInstructions & p_stepIn, GromacsJobData & p_currentJobData )
		{
			fillOutputsFromExpectations( p_in, p_currentJobData );
			if ( auto fileStrPtr = getFirstFileOfType( p_in.outputs, ".top" ) )
			{
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
			}
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
			{
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
			}
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
			{
				p_in.outputs.lastUncompiledTop = *fileStrPtr;
			}
		}

		template<typename Instruction>
		bool carryPreparationStep(
			GromacsInstructions & p_in,
			Instruction &		  p_stepIn,
			const char *		  p_stepName,
			int &				  stepNum
		) noexcept
		{
			const int displayIndex = stepNum;
			App::THREAD().dispatch( [ event = Gateway::PreparationStepStarted { displayIndex, p_stepName } ]() mutable
									{ App::HUB().trigger( std::move( event ) ); } );

			const FilePath jobDir = p_in.rootDir / p_stepName;
			VTX_DEBUG( "[MDPREP] Starting preparation step <{}> in <{}>.", p_stepName, jobDir.string() );
			p_stepIn.fileStem = p_in.fileStem;
			prepareJob( p_in.outputs, p_in.rootDir, p_stepName, p_stepIn );
			auto & currentJobData = p_in.jobData[ stepNum ];
			stepNum++;
			convert( p_stepIn, currentJobData );
			FilePath gmxExe = VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryPath();
			submitGromacsJob( gmxExe, jobDir, currentJobData );
			currentJobData.postJobRoutine( jobDir, currentJobData, p_in.outputs );
			checkJobResults( currentJobData );

			// Snapshot the gromacs output channels so the UI can display them, then hand the snapshot to the main
			// thread through the event. Done in a closed scope to release the channels lock before firing.
			const auto fireStepFinished = [ & ]( bool p_success )
			{
				Gateway::PreparationStepFinished ev;
				ev.index   = displayIndex;
				ev.success = p_success;
				ev.errors  = currentJobData.report.errors;
				{
					auto channels = currentJobData.channelsLocker.open();
					ev.stdOut	  = channels->stdout_;
					ev.stdErr	  = channels->stderr_;
				}
				App::THREAD().dispatch( [ event = std::move( ev ) ]() mutable
										{ App::HUB().trigger( std::move( event ) ); } );
			};

			if ( currentJobData.report.errorOccured )
			{
				VTX_ERROR(
					"[MDPREP] Preparation step <{}> failed with {} error(s).",
					p_stepName,
					currentJobData.report.errors.size()
				);
				for ( const std::string & error : currentJobData.report.errors )
				{
					VTX_ERROR( "[MDPREP] Step <{}>: {}", p_stepName, error );
				}
				{
					auto channels = currentJobData.channelsLocker.open();
					if ( not channels->stderr_.empty() )
					{
						VTX_ERROR( "[MDPREP] Step <{}> stderr:\n{}", p_stepName, channels->stderr_ );
					}
					if ( not channels->stdout_.empty() )
					{
						VTX_ERROR( "[MDPREP] Step <{}> stdout:\n{}", p_stepName, channels->stdout_ );
					}
				}
				fireStepFinished( false );
				return false;
			}

			VTX_DEBUG( "[MDPREP] Preparation step <{}> completed.", p_stepName );
			fillOutputs( p_in, p_stepIn, currentJobData );
			fireStepFinished( true );
			return true;
		}
	} // namespace

	void prepareStructure(
		VTX::Util::Thread::ThreadData & p_thrData,
		const FilePath &				p_structurePdb,
		GromacsInstructions &			p_in
	) noexcept
	{
		p_in.fileStem		  = p_structurePdb.stem().string();
		p_in.pdb2gmx.inputPdb = p_structurePdb;

		int jobIdx = 0;

		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "1 - pdb2gmx" );
		}
		if ( carryPreparationStep( p_in, p_in.pdb2gmx, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "2 - editconf" );
		}
		if ( carryPreparationStep( p_in, p_in.editconf1, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "3 - solvate" );
		}
		if ( carryPreparationStep( p_in, p_in.solvate, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "4 - trjconv" );
		}
		if ( carryPreparationStep( p_in, p_in.trjconv, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "5 - grompp ions" );
		}
		p_in.gromppIons.step = E_GROMPP_STEP::ions;
		if ( carryPreparationStep( p_in, p_in.gromppIons, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "6 - genion" );
		}
		if ( carryPreparationStep( p_in, p_in.genion, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "6 - grompp posres" );
		}
		p_in.gromppPosres.step = E_GROMPP_STEP::posres;
		if ( carryPreparationStep( p_in, p_in.gromppPosres, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "7 - grompp em" );
		}
		p_in.gromppEm.step = E_GROMPP_STEP::em;
		if ( carryPreparationStep( p_in, p_in.gromppEm, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
		if ( p_thrData.thrRef )
		{
			p_thrData.thrRef->get().setProgressText( "8 - editconf" );
		}
		if ( carryPreparationStep( p_in, p_in.editconf2, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_thrData.stopToken.stop_requested() )
		{
			return;
		}
	}

	void createMdDirectory( const GromacsInstructions &, const FilePath & p_dest ) noexcept {}

	class SystemTester::_Impl
	{
		struct TestData
		{
			FilePath		 _structurePdb;
			forcefield		 _ff;
			E_WATER_MODEL	 _w;
			std::atomic_bool _systemOk = false;
			std::string		 _why;
		} _testData;

		static FilePath createRootDir()
		{
			FilePath base = App::SESSION().getAppTmpFolder() / "tools" / "mdprep" / "test", out;
			for ( uint32_t i = 0; i != 0xffffffff; i++ )
			{
				out = base / std::to_string( i );
				if ( fs::exists( out ) == false )
				{
					return out;
				}
			}
			return base / "a"; // ragequit
		}

		void test() {}

	  public:
		_Impl( const FilePath & p_structurePdb, const forcefield & p_ff, const E_WATER_MODEL & p_w ) :
			_testData( TestData { p_structurePdb, p_ff, p_w } )
		{
			FilePath rootDir = createRootDir();
			try
			{
				Pdb2gmxInstructions inst;
				if ( fs::exists( rootDir ) )
				{
					fs::remove_all( rootDir );
				}
				fs::create_directories( rootDir );
				inst.forcefields	  = { _testData._ff };
				inst.forcefieldIndex  = 0;
				inst.water			  = _testData._w;
				inst.fileStem		  = _testData._structurePdb.stem().string();
				inst.inputPdb		  = _testData._structurePdb;
				const FilePath jobDir = rootDir.parent_path() / "1";
				prepareJob( {}, rootDir.parent_path(), "1", inst );
				GromacsJobData jobData;
				convert( inst, jobData );
				declareFfDirectory( VTX::Tool::Mdprep::backends::Gromacs::defaultFfDirectoryPath() );
				submitGromacsJob( VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryPath(), jobDir, jobData );
				checkJobResults( jobData );
				_testData._systemOk = jobData.report.errorOccured == false;
				for ( auto & err : jobData.report.errors )
				{
					_testData._why += err + '\n';
				}
				auto channels = jobData.channelsLocker.open();
				_testData._why += channels->stdout_ + "\n";
				_testData._why += channels->stderr_ + "\n";
			}
			catch ( std::exception & e )
			{
				_testData._why = e.what();
			}
			catch ( ... )
			{
				_testData._why = "Unknown error.";
			}
			fs::remove_all( rootDir );
		}

		bool isSystemOk() const noexcept { return _testData._systemOk; }

		const std::string_view why() const noexcept { return _testData._why; }
	};

	SystemTester::SystemTester( const FilePath & p_structurePdb, const forcefield & p_ff, const E_WATER_MODEL & p_w ) :
		_pimpl( new _Impl( p_structurePdb, p_ff, p_w ) )
	{
	}

	bool SystemTester::isSystemOk() const noexcept { return _pimpl->isSystemOk(); }

	const std::string_view SystemTester::why() const noexcept { return _pimpl->why(); }

	SystemTester::operator bool() const noexcept { return _pimpl->isSystemOk(); }

	void SystemTester::Del::operator()( const _Impl * p ) const { delete p; }

} // namespace VTX::Tool::Mdprep::backends::Gromacs
