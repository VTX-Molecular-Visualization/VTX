#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/action/visibility.hpp>
#include <app/constants.hpp>
#include <app/services.hpp>
#include <app/system/visibility.hpp>
#include <latch>
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
			VTX::FilePath dest { p_instr.rootDir / "init.pdb" };

			App::Action::IO::WriteVisible a;
			App::ACTION().execute( a, VTX::FilePath( dest ) );
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			backends::Gromacs::prepareStructure( _impl->thrData, dest, p_instr );
			VTX::FilePath					  resultDir = p_instr.rootDir / "md_ready";
			backends::Gromacs::MdInstructions packInstructions;
			backends::Gromacs::pack( resultDir, p_instr.outputs, packInstructions );
			if ( _impl->thrData.stopToken.stop_requested() )
			{
				goto theEnd;
			}
			bool noErrors = true;
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
			}
			else
			{
				VTX_ERROR( "Preparation failed." ); // TODO : do better.
			}
		}

		// TODO

	theEnd:
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
