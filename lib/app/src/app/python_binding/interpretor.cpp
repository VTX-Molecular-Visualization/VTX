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
				[ this ]( Util::StopToken p_stopToken, App::Core::Threading::BaseThread & p_thread )
				{ return runPythonThread( p_stopToken, p_thread ); }
			) )
		{
		}
		~_Impl()
		{
			while ( not _threadedLoopStarted )
				std::this_thread::sleep_for( _inactivitySleepTime.load() );
			_thread->stop();
			while ( not _threadedLoopFinished )
				std::this_thread::sleep_for( _inactivitySleepTime.load() );
		}

		inline void runCommand( const std::string & p_command ) noexcept
		{
			auto queue = _lockedCmdQueue.open();
			queue->push( p_command );
		}
		inline void runCommand( const std::string & p_command, std::future<AsyncJobResult> & p_ret ) noexcept
		{
			_commandReturnValue.emplace();
			p_ret = _commandReturnValue->get_future();
			runCommand( p_command );
		}

		inline void runScript( const FilePath & p_path )
		{
			if ( std::this_thread::get_id() != _thread->getId() )
			{
				subscribe( [ this, path = p_path ]( VTX::PythonBinding::Interpretor & ) { _actuallyRunScript( path ); }
				);
			}
			else
			{
				_actuallyRunScript( p_path );
			}
		}
		inline void runScript( const FilePath & p_path, std::future<AsyncJobResult> & p_future )
		{
			// This method can be called from python thread directly. In this specific scenario, we don't need to use
			// the subscription/callback infrastructure.
			_scriptReturn.emplace();
			p_future = _scriptReturn->get_future();
			runScript( p_path );
		}

		inline void slowerResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 1000 ); }
		inline void fasterResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 100 ); }
		inline void subscribe( InterpretorInstructionsOneShot p_instruction ) noexcept
		{
			_instructions += std::move( p_instruction );
		}

		inline int runPythonThread( Util::StopToken p_stopToken, App::Core::Threading::BaseThread & _ )
		{
			_stopToken = std::move( p_stopToken );
			_thread	   = &_;
			_interpretor.emplace();
			_threadedLoopStarted = true;
			this->_listenQueue();
			_interpretor.reset();
			_threadedLoopFinished = true;
			return 0;
		}

	  private:
		inline void _listenQueue()
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
		}
		inline void _actuallyRunCommand( const std::string & p_command )
		{
			std::string rslt;
			bool		success = false;
			try
			{
				VTX_PYTHON_IN( "{}", p_command );
				rslt	= _interpretor->runCommand( p_command );
				success = true;
				if ( not rslt.empty() )
					VTX_PYTHON_OUT( "{}", rslt );
			}
			catch ( CommandException & p_e )
			{
				VTX_PYTHON_OUT( "{}", p_e.what() );
				rslt = p_e.what();
			}
			catch ( ScriptException & p_e )
			{
				VTX_PYTHON_OUT( "{}", p_e.what() );
				rslt = p_e.what();
			}

			if ( _commandReturnValue )
				_commandReturnValue->set_value( { success, rslt } );

			_commandReturnValue.reset();
		}
		inline void _actuallyRunScript( const FilePath & p_path )
		{
			try
			{
				_interpretor->runScript( p_path );
				if ( _scriptReturn )
					_scriptReturn->set_value( { true } );
			}
			catch ( VTX::ScriptException & e )
			{
				if ( _scriptReturn )
					_scriptReturn->set_value( { false, e.what() } );
				VTX_ERROR( "Error while running script : {}", e.what() );
			}
			_scriptReturn.reset();
		}

		std::atomic<std::chrono::milliseconds> _inactivitySleepTime { std::chrono::milliseconds( 100 ) };
		std::atomic_bool					   _threadedLoopStarted = false;
		std::atomic_bool					   _threadedLoopFinished
			= false; // Used to inform the main thread that the python thread has finished
		std::optional<VTX::PythonBinding::Interpretor>
			_interpretor; // Optional because it will be created and destroyed in the python thread
		Util::DataLocker<std::queue<std::string>>		  _lockedCmdQueue;
		App::Core::Threading::BaseThread *				  _thread = nullptr;
		Util::StopToken									  _stopToken;
		std::optional<std::promise<AsyncJobResult>>		  _commandReturnValue;
		std::optional<std::promise<AsyncJobResult>>		  _scriptReturn;
		Util::Callback<VTX::PythonBinding::Interpretor &> _instructions;
	};

	Interpretor::Interpretor() : _impl( new _Impl() ) {}
	void Interpretor::runCommand( const std::string & p_ ) noexcept { _impl->runCommand( p_ ); }
	void Interpretor::runCommand( const std::string & p_cmd, std::future<AsyncJobResult> & p_ret ) noexcept
	{
		_impl->runCommand( p_cmd, p_ret );
	}
	void Interpretor::runScript( const FilePath & p_path ) noexcept { _impl->runScript( p_path ); }
	void Interpretor::runScript( const FilePath & p_path, std::future<AsyncJobResult> & p_future ) noexcept
	{
		_impl->runScript( p_path, p_future );
	}
	// bool Interpretor::lastScriptFailed() const { return _impl->lastScriptFailed(); }
	void Interpretor::slowerResponseTime() noexcept { _impl->slowerResponseTime(); }
	void Interpretor::fasterResponseTime() noexcept { _impl->fasterResponseTime(); }
	void Interpretor::subscribe( InterpretorInstructionsOneShot _ ) noexcept { _impl->subscribe( std::move( _ ) ); }
	void Interpretor::Del::operator()( Interpretor::_Impl * p_ ) noexcept { delete p_; }
} // namespace VTX::App::PythonBinding
