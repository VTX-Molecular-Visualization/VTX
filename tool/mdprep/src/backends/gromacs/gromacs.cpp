#include <latch>
#include <thread>
//
#include <tool/mdprep/backends/gromacs/gromacs.hpp>

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
			{
				return false;
			}

			fillOutputs( p_in, p_stepIn, currentJobData );
			return true;
		}
	} // namespace

	void prepareStructure(
		Util::StopToken		  p_token,
		const fs::path &	  p_structurePdb,
		GromacsInstructions & p_in
	) noexcept
	{
		p_in.fileStem		  = p_structurePdb.stem().string();
		p_in.pdb2gmx.inputPdb = p_structurePdb;

		int jobIdx = 0;

		if ( carryPreparationStep( p_in, p_in.pdb2gmx, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		if ( carryPreparationStep( p_in, p_in.editconf1, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		if ( carryPreparationStep( p_in, p_in.solvate, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		if ( carryPreparationStep( p_in, p_in.trjconv, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		p_in.gromppIons.step = E_GROMPP_STEP::ions;
		if ( carryPreparationStep( p_in, p_in.gromppIons, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		if ( carryPreparationStep( p_in, p_in.genion, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		p_in.gromppPosres.step = E_GROMPP_STEP::posres;
		if ( carryPreparationStep( p_in, p_in.gromppPosres, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		p_in.gromppEm.step = E_GROMPP_STEP::em;
		if ( carryPreparationStep( p_in, p_in.gromppEm, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
		if ( carryPreparationStep( p_in, p_in.editconf2, g_jobNames[ jobIdx ], jobIdx ) == false
			 || p_token.stop_requested() )
		{
			return;
		}
	}

	void createMdDirectory( const GromacsInstructions &, const fs::path & p_dest ) noexcept {}

	class SystemTester::_Impl
	{
		struct TestData
		{
			fs::path		 _structurePdb;
			forcefield		 _ff;
			E_WATER_MODEL	 _w;
			std::latch		 _finishedLatch { 1 };
			std::atomic_bool _systemOk = false;
			std::string		 _why;
		} _testData;

		std::thread _thr;

		static fs::path createRootDir()
		{
			fs ::path base = fs::temp_directory_path() / "VTX" / "tools" / "mdprep" / "test", out;
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

		static void test( TestData & p_testData )
		{
			fs::path rootDir = createRootDir();
			try
			{
				Pdb2gmxInstructions inst;
				if ( fs::exists( rootDir ) )
				{
					fs::remove_all( rootDir );
				}
				fs::create_directories( rootDir );
				inst.forcefields	 = { p_testData._ff };
				inst.forcefieldIndex = 0;
				inst.water			 = p_testData._w;
				inst.fileStem		 = p_testData._structurePdb.stem().string();
				inst.inputPdb		 = p_testData._structurePdb;
				prepareJob( {}, rootDir.parent_path(), "1", inst );
				GromacsJobData jobData;
				convert( inst, jobData );
				declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
				submitGromacsJob(
					VTX::Tool::Mdprep::executableDirectory()
						/ VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryRelativePath(),
					jobData
				);
				checkJobResults( jobData );
				p_testData._finishedLatch.count_down();
				p_testData._systemOk = jobData.report.errorOccured == false;
				for ( auto & err : jobData.report.errors )
				{
					p_testData._why += err + '\n';
				}
			}
			catch ( std::exception & e )
			{
				p_testData._why = e.what();
			}
			p_testData._finishedLatch.count_down();
			fs::remove_all( rootDir );
		}

		static std::thread startTest( TestData & p_testData )
		{
			return std::thread( [ & ]() { test( p_testData ); } );
		}

	  public:
		_Impl( const fs::path & p_structurePdb, const forcefield & p_ff, const E_WATER_MODEL & p_w ) :
			_testData( TestData { p_structurePdb, p_ff, p_w } )
		{
		}

		~_Impl()
		{
			if ( _thr.joinable() )
			{
				_thr.join();
			}
		}

		bool isTestFinished() const noexcept { return _testData._finishedLatch.try_wait(); }

		bool isSystemOk() const noexcept
		{
			_testData._finishedLatch.wait();
			return _testData._systemOk;
		}

		const std::string_view why() const noexcept
		{
			if ( isTestFinished() || _testData._systemOk == true )
			{
				return {};
			}
			return _testData._why;
		}
	};

	SystemTester::SystemTester( const fs::path & p_structurePdb, const forcefield & p_ff, const E_WATER_MODEL & p_w ) :
		_pimpl( new _Impl( p_structurePdb, p_ff, p_w ) )
	{
	}

	bool SystemTester::isTestFinished() const noexcept { return _pimpl->isTestFinished(); }

	bool SystemTester::isSystemOk() const noexcept { return _pimpl->isSystemOk(); }

	const std::string_view SystemTester::why() const noexcept { return _pimpl->why(); }

	SystemTester::operator bool() const noexcept { return _pimpl->isSystemOk(); }

	void SystemTester::Del::operator()( const _Impl * p ) const { delete p; }

} // namespace VTX::Tool::Mdprep::backends::Gromacs
