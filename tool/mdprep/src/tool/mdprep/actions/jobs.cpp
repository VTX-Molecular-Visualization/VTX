#include <array>
#include <cstdlib>
#include <latch>
#include <string_view>
//
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/action/visibility.hpp>
#include <app/constants.hpp>
#include <app/services.hpp>
#include <app/system/visibility.hpp>
#include <app/threading/trigger_event.hpp>
//
#include <tool/mdprep/actions/jobs.hpp>
#include <tool/mdprep/backends/gromacs/gromacs.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>
#include <tool/mdprep/gateway/shared.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Mdprep::Actions
{
	struct StartPreparation::_Impl
	{
		VTX::App::Threading::ThreadData thrData;
		std::latch						waiter { 1 };
	};

	namespace
	{
		constexpr std::array<const char *, backends::Gromacs::g_NUM_PREPARATION_JOBS> JOB_NAMES {
			"pdb2gmx", "editconf",		"solvate",	 "trjconv",			"grompp ions",
			"genion",  "grompp posres", "grompp em", "editconf output",
		};

		std::string _formatArguments( const std::vector<std::string> & p_arguments )
		{
			std::string result;
			for ( const std::string & argument : p_arguments )
			{
				if ( not result.empty() )
				{
					result += ' ';
				}
				result += fmt::format( "\"{}\"", argument );
			}
			return result;
		}

		void _logPathState( const std::string_view p_label, const fs::path & p_path )
		{
			std::error_code error;
			const bool		exists = fs::exists( p_path, error );
			if ( error )
			{
				VTX_ERROR( "{}: <{}> (status error: {})", p_label, p_path.string(), error.message() );
				return;
			}

			const bool isFile	   = exists && fs::is_regular_file( p_path, error );
			const bool isDirectory = exists && fs::is_directory( p_path, error );
			if ( error )
			{
				VTX_ERROR( "{}: <{}> (type error: {})", p_label, p_path.string(), error.message() );
				return;
			}

			uintmax_t size = 0;
			if ( isFile )
			{
				size = fs::file_size( p_path, error );
			}

			VTX_ERROR(
				"{}: <{}>, exists={}, regular_file={}, directory={}, size={}{}",
				p_label,
				p_path.string(),
				exists,
				isFile,
				isDirectory,
				size,
				error ? fmt::format( ", size error: {}", error.message() ) : std::string {}
			);
		}

		void _logDirectoryContent( const fs::path & p_directory )
		{
			VTX_ERROR( "Preparation directory content for <{}>:", p_directory.string() );

			std::error_code					 error;
			fs::recursive_directory_iterator iterator(
				p_directory, fs::directory_options::skip_permission_denied, error
			);
			const fs::recursive_directory_iterator end;
			if ( error )
			{
				VTX_ERROR( "  Unable to enumerate directory: {}", error.message() );
				return;
			}

			for ( ; iterator != end; iterator.increment( error ) )
			{
				if ( error )
				{
					VTX_ERROR( "  Directory iteration error: {}", error.message() );
					error.clear();
					continue;
				}

				const fs::directory_entry & entry = *iterator;
				std::error_code				entryError;
				const bool					isFile = entry.is_regular_file( entryError );
				const uintmax_t				size   = isFile ? entry.file_size( entryError ) : 0;
				VTX_ERROR(
					"  <{}>, regular_file={}, size={}{}",
					entry.path().string(),
					isFile,
					size,
					entryError ? fmt::format( ", status error: {}", entryError.message() ) : std::string {}
				);
			}
		}

		void _logPreparationFailure(
			backends::Gromacs::GromacsInstructions & p_instructions,
			const backends::Gromacs::ErrorReport &	 p_packReport,
			const fs::path &						 p_input,
			const fs::path &						 p_resultDirectory
		)
		{
			VTX_ERROR( "Preparation failed." );
			VTX_ERROR( "File stem: <{}>", p_instructions.fileStem );
			_logPathState( "Working directory", p_instructions.rootDir );
			_logPathState( "Input structure", p_input );
			_logPathState( "Result directory", p_resultDirectory );
			_logPathState(
				"Gromacs executable", Mdprep::executableDirectory() / backends::Gromacs::defaultGmxBinaryRelativePath()
			);
			_logPathState(
				"Gromacs force-field directory",
				Mdprep::executableDirectory() / backends::Gromacs::defaultFfDirectoryRelativePath()
			);
			_logPathState(
				"Gromacs templates directory",
				Mdprep::executableDirectory() / backends::Gromacs::defaultGmxTemplatesRelativePath()
			);
			const char * const gmxlib = std::getenv( "GMXLIB" );
			VTX_ERROR( "GMXLIB=<{}>", gmxlib == nullptr ? "<unset>" : gmxlib );

			VTX_ERROR( "Pack report: error={}, message=<{}>", p_packReport.error, p_packReport.errMsg );
			VTX_ERROR(
				"Cumulative outputs: count={}, last uncompiled topology=<{}>",
				p_instructions.outputs.fileStringPtrs.size(),
				p_instructions.outputs.lastUncompiledTop.string()
			);
			for ( size_t outputIndex = 0; outputIndex < p_instructions.outputs.fileStringPtrs.size(); ++outputIndex )
			{
				const std::string * const output = p_instructions.outputs.fileStringPtrs[ outputIndex ];
				if ( output == nullptr )
				{
					VTX_ERROR( "Cumulative output [{}]: null", outputIndex );
					continue;
				}
				_logPathState( fmt::format( "Cumulative output [{}]", outputIndex ), *output );
			}

			for ( size_t jobIndex = 0; jobIndex < p_instructions.jobData.size(); ++jobIndex )
			{
				auto & job = p_instructions.jobData[ jobIndex ];
				VTX_ERROR(
					"Job [{}] {}: finished={}, error={}, arguments={}, expected_outputs={}, reported_errors={}",
					jobIndex,
					JOB_NAMES[ jobIndex ],
					job.report.finished,
					job.report.errorOccured,
					job.arguments.size(),
					job.expectedOutputFilesIndexes.size(),
					job.report.errors.size()
				);
				VTX_ERROR( "Job [{}] command: {}", jobIndex, _formatArguments( job.arguments ) );

				for ( size_t errorIndex = 0; errorIndex < job.report.errors.size(); ++errorIndex )
				{
					VTX_ERROR(
						"Job [{}] report error [{}]: {}", jobIndex, errorIndex, job.report.errors[ errorIndex ]
					);
				}

				for ( const size_t argumentIndex : job.expectedOutputFilesIndexes )
				{
					if ( argumentIndex >= job.arguments.size() )
					{
						VTX_ERROR(
							"Job [{}] expected output references invalid argument index {} (argument count={})",
							jobIndex,
							argumentIndex,
							job.arguments.size()
						);
						continue;
					}
					_logPathState(
						fmt::format( "Job [{}] expected output argument [{}]", jobIndex, argumentIndex ),
						job.arguments[ argumentIndex ]
					);
				}

				auto channels = job.channelsLocker.open();
				VTX_ERROR(
					"Job [{}] stdout ({} bytes):\n{}",
					jobIndex,
					channels->stdout_.size(),
					channels->stdout_.empty() ? "<empty>" : channels->stdout_
				);
				VTX_ERROR(
					"Job [{}] stderr ({} bytes):\n{}",
					jobIndex,
					channels->stderr_.size(),
					channels->stderr_.empty() ? "<empty>" : channels->stderr_
				);
			}

			_logDirectoryContent( p_instructions.rootDir );
		}
	} // namespace

	void StartPreparation::_Del::operator()( _Impl * _ptr ) const noexcept { delete _ptr; }

	StartPreparation::StartPreparation() : _impl( new _Impl ) {}

	StartPreparation::StartPreparation( VTX::App::Threading::ThreadData p_ ) : _impl( new _Impl { std::move( p_ ) } ) {}

	void StartPreparation::execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions p_instr )
	{
		if ( not App::System::isAnythingVisible() )
		{
			goto theEnd;
		}
		{
			VTX::FilePath dest { p_instr.rootDir
								 / "your_system.pdb" }; // TODO bad design (see backend_gromacs.cpp:convert)

			App::Action::IO::WriteVisible a;
			App::ACTION().execute( a, VTX::FilePath( dest ) );
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			backends::Gromacs::prepareStructure( _impl->thrData, dest, p_instr );
			VTX::FilePath						 resultDir = p_instr.rootDir / "md_ready";
			const backends::Gromacs::ErrorReport packReport
				= backends::Gromacs::pack( resultDir, p_instr.outputs, p_instr.mdInstructions );
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			bool noErrors = not packReport.error;
			for ( auto & jobData : p_instr.jobData )
			{
				noErrors &= not jobData.report.errorOccured;
			}
			if ( noErrors )
			{
				App::ACTION().execute<App::Action::IO::LoadSystem>( p_instr.editconf2.out );
				App::ACTION().execute<App::Action::Visibility::HideEverything>();
				VTX_INFO(
					"System written at : {}",
					fmt::format( fmt::runtime( std::string( App::LOG_LINK_FORMAT ) ), resultDir.string() )
				);
				App::HUB().trigger( PreparationFinished { true } );
			}
			else
			{
				_logPreparationFailure( p_instr, packReport, dest, resultDir );
			}
		}

	theEnd:
		/**/ {
			App::Threading::TiggerEvent t { PreparationFinished() };
		}
		_impl->waiter.count_down();
	}

	void StartPreparation::wait() noexcept
	{
		if ( _impl )
		{
			_impl->waiter.wait();
		}
	}

	namespace
	{

		class TriggerCheckReportEvent
		{
		  public:
			inline void execute( const Mdprep::Gateway::CheckReport & p_ )
			{ App::HUB().trigger<Mdprep::Gateway::CheckReport>( p_ ); }
		};
	} // namespace

	struct CheckSystem::_Impl
	{
		VTX::App::Threading::ThreadData thrData;
		std::latch						waiter { 1 };
	};

	CheckSystem::CheckSystem() : _impl( new _Impl ) {}

	CheckSystem::CheckSystem( VTX::App::Threading::ThreadData p_ ) : _impl( new _Impl { std::move( p_ ) } ) {}

	void CheckSystem::execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions p_gmxIntructions )
	{
		if ( not App::System::isAnythingVisible() )
		{
			App::ACTION().subscribe(
				App::Action::QueuedAction(
					TriggerCheckReportEvent(),
					Mdprep::Gateway::CheckReport {
						Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField, 0, true, "Nothing to check." }
				)
			);
			goto theEnd;
		}
		if ( _impl->thrData.thrRef )
		{
			_impl->thrData.thrRef->get().setProgressText( "Checking system for MD" );
		}
		{
			VTX::FilePath				  dest { p_gmxIntructions.rootDir / "test.pdb" };
			App::Action::IO::WriteVisible a;
			App::ACTION().execute( a, VTX::FilePath( dest ) );

			backends::Gromacs::SystemTester tester(
				dest,
				p_gmxIntructions.pdb2gmx.forcefields[ p_gmxIntructions.pdb2gmx.forcefieldIndex ],
				p_gmxIntructions.pdb2gmx.water
			);
			auto reason = tester.why();
			App::ACTION().subscribe(
				App::Action::QueuedAction(
					TriggerCheckReportEvent(),
					Mdprep::Gateway::CheckReport { Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField,
												   0,
												   tester.isSystemOk(),
												   std::string( reason.begin(), reason.end() ) }
				)
			);
		}

	theEnd:
		_impl->waiter.count_down();
	}

	void CheckSystem::wait() noexcept
	{
		if ( _impl )
		{
			_impl->waiter.wait();
		}
	}

	void CheckSystem::_Del::operator()( _Impl * _ptr ) const noexcept { delete _ptr; }
} // namespace VTX::Tool::Mdprep::Actions
