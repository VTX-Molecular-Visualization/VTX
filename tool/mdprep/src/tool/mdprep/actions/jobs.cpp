#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <latch>
#include <tool/mdprep/actions/jobs.hpp>
#include <tool/mdprep/backends/gromacs/gromacs.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>
#include <tool/mdprep/gateway/shared.hpp>
#include <util/event_hub.hpp>

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

	void StartPreparation::execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions )
	{
		// TODO
		_impl->waiter.count_down();
	}

	void StartPreparation::wait() noexcept
	{
		if ( _impl )
		{
			_impl->waiter.wait();
		}
	}

	struct CheckSystem::_Impl
	{
		VTX::App::Threading::ThreadData thrData;
		std::latch						waiter { 1 };
	};

	CheckSystem::CheckSystem() : _impl( new _Impl ) {}

	CheckSystem::CheckSystem( VTX::App::Threading::ThreadData p_ ) : _impl( new _Impl { std::move( p_ ) } ) {}

	void CheckSystem::execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions p_gmxIntructions )
	{
		VTX::FilePath				  dest { backends::Gromacs::createNewEmptyTempDirectory() / "test.pdb" };
		App::Action::IO::WriteVisible a;
		App::ACTION().execute( a, std::move( dest ) );

		backends::Gromacs::SystemTester tester(
			dest,
			p_gmxIntructions.pdb2gmx.forcefields[ p_gmxIntructions.pdb2gmx.forcefieldIndex ],
			p_gmxIntructions.pdb2gmx.water
		);
		auto reason = tester.why();
		App::ACTION().subscribe(
			Mdprep::Gateway::CheckReport { Gateway::E_REPORT_CHECKED_ITEM::systemWithForceField,
										   0,
										   tester.isSystemOk(),
										   std::string( reason.begin(), reason.end() ) }
		);
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
