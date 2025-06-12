#include "app/python_binding/interpretor.hpp"
#include "app/core/threading/threading_system.hpp"
#include <atomic>
#include <optional>
#include <python_binding/interpretor.hpp>
#include <queue>
#include <util/datalocker.hpp>
#include <util/logger.hpp>

namespace VTX::App::PythonBinding
{
	class Interpretor::_Impl
	{
	  public:
		_Impl() :
			_thread( &THREADING_SYSTEM().createThread(
				[ this ]( Util::StopToken p_stopToken, App::Core::Threading::BaseThread & _ )
				{
					_interpretor.emplace();
					this->_stopToken = std::move( p_stopToken );
					this->listenQueue( _ );
					return 0;
				}
			) )
		{
		}

		void runCommand( const std::string & p_command )
		{
			auto queue = _lockedCmdQueue.open();
			queue->push( p_command );
		}

		void listenQueue( App::Core::Threading::BaseThread & p_vtxThread )
		{
			std::string command;
			while ( true )
			{
			queue_check:
			{
				auto queue = _lockedCmdQueue.open();
				if ( not queue->empty() )
				{
					command = queue->front();
					queue->pop();
				}
			}
				if ( not command.empty() )
				{
					_actuallyRunCommand( command );
					command.clear();
					goto queue_check; // If there was a command in the queue, maybe there is others as well so we don't
									  // want to wait the time interval before executing it.
				}

				std::this_thread::sleep_for( _inactivitySleepTime.load() );
				if ( _stopToken.stop_requested() )
					break;
			}
		}
		VTX::PythonBinding::Interpretor & pythonInterpretor() { return *_interpretor; }
		inline void slowerResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 1000 ); }
		inline void fasterResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 100 ); }

	  private:
		void _actuallyRunCommand( const std::string & p_command )
		{
			try
			{
				VTX_PYTHON_IN( "{}", p_command );
				std::string rslt = _interpretor->runCommand( p_command );
				if ( not rslt.empty() )
					VTX_PYTHON_OUT( "{}", rslt );
			}
			catch ( CommandException & p_e )
			{
				VTX_PYTHON_OUT( "{}", p_e.what() );
			}
		}

		std::atomic<std::chrono::milliseconds>		   _inactivitySleepTime { std::chrono::milliseconds( 100 ) };
		std::optional<VTX::PythonBinding::Interpretor> _interpretor;
		Util::DataLocker<std::queue<std::string>>	   _lockedCmdQueue;
		App::Core::Threading::BaseThread *			   _thread = nullptr;
		Util::StopToken								   _stopToken;
	};

	Interpretor::Interpretor() : _impl( new _Impl() ) {}
	void Interpretor::runCommand( const std::string & p_ ) { _impl->runCommand( p_ ); }
	void Interpretor::slowerResponseTime() noexcept { _impl->slowerResponseTime(); }
	void Interpretor::fasterResponseTime() noexcept { _impl->fasterResponseTime(); }
	void Interpretor::Del::operator()( Interpretor::_Impl * p_ ) { delete p_; }
} // namespace VTX::App::PythonBinding
