#include <python_binding/interpretor.hpp>
//
#include "app/core/threading/threading_system.hpp"
#include "app/python_binding/interpretor.hpp"
#include <atomic>
#include <optional>
#include <queue>
#include <util/callback.hpp>
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
					_stopToken = std::move( p_stopToken );
					_thread	   = &_;
					this->listenQueue();
					_interpretor.reset();
					return 0;
				}
			) )
		{
		}
		~_Impl()
		{
			_thread->stop();
			while ( not _threadedLoopFinished )
				std::this_thread::sleep_for( _inactivitySleepTime.load() );
		}

		void runCommand( const std::string & p_command ) noexcept
		{
			auto queue = _lockedCmdQueue.open();
			queue->push( p_command );
		}

		void listenQueue()
		{
			std::string command;
			while ( true )
			{
			loop_beginning:
				_instructions( *_interpretor );
				_instructions.clear();
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
					goto loop_beginning; // If there was a command in the queue, maybe there is others as well so we
										 // don't want to wait the time interval before executing it.
				}

				std::this_thread::sleep_for( _inactivitySleepTime.load() );
				if ( _stopToken.stop_requested() )
					break;
			}
			_threadedLoopFinished = true;
		}
		VTX::PythonBinding::Interpretor & pythonInterpretor() { return *_interpretor; }
		inline void slowerResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 1000 ); }
		inline void fasterResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 100 ); }
		inline void subscribe( InterpretorInstructionsOneShot p_instruction ) noexcept
		{
			_instructions += std::move( p_instruction );
		}
		void runCommand( const std::string & p_command, std::future<std::string> & p_ret ) noexcept
		{
			_commandReturnValue.emplace();
			p_ret = _commandReturnValue->get_future();
			runCommand( p_command );
		}
		bool lastCommandFailed() const { return _lastCommandFailed.load(); }

	  private:
		void _actuallyRunCommand( const std::string & p_command )
		{
			std::string rslt;
			try
			{
				VTX_PYTHON_IN( "{}", p_command );
				rslt			   = _interpretor->runCommand( p_command );
				_lastCommandFailed = false;
				if ( not rslt.empty() )
					VTX_PYTHON_OUT( "{}", rslt );
			}
			catch ( CommandException & p_e )
			{
				_lastCommandFailed = true;
				VTX_PYTHON_OUT( "{}", p_e.what() );
			}

			if ( _commandReturnValue )
				_commandReturnValue->set_value( rslt );
			_commandReturnValue.reset();
		}

		std::atomic<std::chrono::milliseconds> _inactivitySleepTime { std::chrono::milliseconds( 100 ) };
		std::atomic_bool					   _lastCommandFailed	 = false;
		std::atomic_bool					   _threadedLoopFinished = false;
		std::optional<VTX::PythonBinding::Interpretor>
			_interpretor; // Optional because it will be created and destroyed in the python thread
		Util::DataLocker<std::queue<std::string>>		  _lockedCmdQueue;
		App::Core::Threading::BaseThread *				  _thread = nullptr;
		Util::StopToken									  _stopToken;
		Util::Callback<VTX::PythonBinding::Interpretor &> _instructions;
		std::optional<std::promise<std::string>>		  _commandReturnValue;
	};

	Interpretor::Interpretor() : _impl( new _Impl() ) {}
	void Interpretor::runCommand( const std::string & p_ ) noexcept { _impl->runCommand( p_ ); }
	void Interpretor::runCommand( const std::string & p_cmd, std::future<std::string> & p_ret ) noexcept
	{
		_impl->runCommand( p_cmd, p_ret );
	}
	bool Interpretor::lastCommandFailed() const
	{
		return _impl->lastCommandFailed();
		;
	}
	void Interpretor::slowerResponseTime() noexcept { _impl->slowerResponseTime(); }
	void Interpretor::fasterResponseTime() noexcept { _impl->fasterResponseTime(); }
	void Interpretor::subscribe( InterpretorInstructionsOneShot _ ) noexcept { _impl->subscribe( std::move( _ ) ); }
	void Interpretor::Del::operator()( Interpretor::_Impl * p_ ) noexcept { delete p_; }
} // namespace VTX::App::PythonBinding
