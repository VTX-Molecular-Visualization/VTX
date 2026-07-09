#include <latch>
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
	}

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
			FilePath dest { p_instr.rootDir / "your_system.pdb" }; // TODO bad design (see backend_gromacs.cpp:convert)

			App::Action::IO::WriteVisible a;
			App::ACTION().execute( a, FilePath( dest ) );
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			backends::Gromacs::prepareStructure( _impl->thrData, dest, p_instr );
			FilePath							 resultDir = p_instr.rootDir / "md_ready";
			backends::Gromacs::MdInstructions	 packInstructions;
			const backends::Gromacs::ErrorReport packReport
				= backends::Gromacs::pack( resultDir, p_instr.outputs, p_instr.mdInstructions );
			if ( packReport.error )
			{
				VTX_ERROR( "[MDPREP] Packing failed: {}", packReport.errMsg );
			}
			App::Threading::TiggerEvent { Gateway::SystemPacked { not packReport.error, resultDir.string() } };
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			bool noErrors = true;
			for ( size_t jobIndex = 0; jobIndex < p_instr.jobData.size(); jobIndex++ )
			{
				auto & jobData = p_instr.jobData[ jobIndex ];
				noErrors &= not jobData.report.errorOccured;
				if ( jobData.report.errorOccured )
				{
					VTX_ERROR(
						"[MDPREP] Job {} failed (finished: {}, errors: {}).",
						jobIndex,
						jobData.report.finished,
						jobData.report.errors.size()
					);
					for ( const std::string & error : jobData.report.errors )
					{
						VTX_ERROR( "[MDPREP] Job {}: {}", jobIndex, error );
					}
				}
			}
			if ( noErrors )
			{
				App::ACTION().execute<App::Action::IO::LoadSystem>( p_instr.editconf2.out );
				App::ACTION().execute<App::Action::Visibility::HideEverything>();
				VTX_INFO(
					"System written at : {}",
					fmt::format( fmt::runtime( std::string( App::LOG_LINK_FORMAT ) ), resultDir.string() )
				);
				App::HUB().trigger( Gateway::PreparationFinished { true } );
			}
			else
			{
				VTX_ERROR( "Preparation failed." ); // TODO : do better.
			}
		}

	theEnd:
		/**/ {
			App::Threading::TiggerEvent t { Gateway::PreparationFinished() };
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
			FilePath					  dest { p_gmxIntructions.rootDir / "test.pdb" };
			App::Action::IO::WriteVisible a;
			App::ACTION().execute( a, FilePath( dest ) );

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
