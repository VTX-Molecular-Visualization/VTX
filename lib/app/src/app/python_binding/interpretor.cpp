#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include <atomic>
#include <optional>
#include <python_binding/interpretor.hpp>
#include <queue>
#include <util/datalocker.hpp>
#include <util/logger.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::PythonBinding
{
	namespace
	{
		struct WaitingPythonCommand
		{
			std::string												   commandStr;
			std::shared_ptr<std::promise<Interpretor::AsyncJobResult>> promise = nullptr;
		};
	} // namespace

	class Interpretor::_Impl
	{
	  public:
		_Impl() :
			_thread( &THREAD().createThread(
				[ this ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & p_thread )
				{ return runPythonThread( p_stopToken, p_thread ); }
			) )
		{
			_thread->setProgressCallback( {} );
			_thread->setTerminatedCallback( {} );
		}

		~_Impl()
		{
			while ( not _threadedLoopStarted )
			{
				std::this_thread::sleep_for( _inactivitySleepTime.load() );
			}
			_thread->stop();
			_thread->wait();
		}

		inline void runCommand( const std::string & p_command ) noexcept
		{
			auto queue = _lockedCmdQueue.open();
			queue->push( { p_command } );
		}

		inline void runCommand(
			const std::string &							  p_command,
			std::shared_ptr<std::promise<AsyncJobResult>> p_ret
		) noexcept
		{
			auto queue = _lockedCmdQueue.open();
			queue->push( { p_command, std::move( p_ret ) } );
		}

		inline void runScript( const FilePath & p_path, std::shared_ptr<std::promise<AsyncJobResult>> p_promise )
		{
			// This method can be called from python thread directly. In this specific scenario, we don't need to use
			// the subscription/callback infrastructure.
			if ( std::this_thread::get_id() != _thread->getId() )
			{
				subscribe( [ this, path = p_path, promise = p_promise ]( VTX::PythonBinding::Interpretor & )
						   { _actuallyRunScript( path, promise ); } );
			}
			else
			{
				_actuallyRunScript( p_path, p_promise );
			}
		}

		inline void runScript( const FilePath & p_path ) { runScript( p_path, nullptr ); }

		inline void slowerResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 1000 ); }

		inline void fasterResponseTime() noexcept { _inactivitySleepTime = std::chrono::milliseconds( 100 ); }

		inline void subscribe( InterpretorInstructionsOneShot p_instruction ) noexcept
		{
			auto instructions = _lockedInstructions.open();
			instructions->push( std::move( p_instruction ) );
		}

		inline int runPythonThread( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & _ )
		{
			_stopToken = std::move( p_stopToken );
			_thread	   = &_;
			try
			{
				_interpretor.emplace();
				_threadedLoopStarted = true;
				this->_listenQueue();
				_interpretor.reset();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Unhandled exception in python thread: {}", p_e.what() );
				_threadedLoopStarted = true;
			}
			return 0;
		}

	  private:
		inline void _listenQueue()
		{
			WaitingPythonCommand		   command;
			InterpretorInstructionsOneShot instruction;
			while ( true )
			{
			loop_beginning:
				command		= WaitingPythonCommand();
				instruction = InterpretorInstructionsOneShot();
				{
					auto instructions = _lockedInstructions.open();
					if ( not instructions->empty() )
					{
						instruction = std::move( instructions->front() );
						instructions->pop();
					}
				}
				if ( instruction )
				{
					instruction( *_interpretor );
					goto loop_beginning;
				}
				{
					auto queue = _lockedCmdQueue.open();
					if ( not queue->empty() )
					{
						command = std::move( queue->front() );
						queue->pop();
					}
				}
				if ( not command.commandStr.empty() )
				{
					_actuallyRunCommand( command );
					goto loop_beginning; // If there was a command in the queue, maybe there is others as well so we
										 // don't want to wait the time interval before executing it.
				}

				std::this_thread::sleep_for( _inactivitySleepTime.load() );
				if ( _stopToken.stop_requested() )
				{
					break;
				}
			}
		}

		inline void _actuallyRunCommand( WaitingPythonCommand & p_command )
		{
			AsyncJobResult jobResult;
			try
			{
				VTX_PYTHON_IN( "{}", p_command.commandStr );
				jobResult.resultStr = _interpretor->runCommand( p_command.commandStr );
				jobResult.success	= true;
				if ( not jobResult.resultStr.empty() )
				{
					VTX_PYTHON_OUT( "{}", jobResult.resultStr );
				}
			}
			catch ( CommandException & p_e )
			{
				VTX_PYTHON_OUT( "Command exception raised in python command : {}", p_e.what() );
				jobResult.resultStr = p_e.what();
			}
			catch ( ScriptException & p_e )
			{
				VTX_PYTHON_OUT( "Script exception raised in python command : {}", p_e.what() );
				jobResult.resultStr = p_e.what();
			}

			if ( p_command.promise )
			{
				p_command.promise->set_value( std::move( jobResult ) );
			}
		}

		inline void _actuallyRunScript(
			const FilePath &							  p_path,
			std::shared_ptr<std::promise<AsyncJobResult>> p_promise
		)
		{
			AsyncJobResult jobResult;
			try
			{
				_interpretor->runScript( p_path );
				jobResult.success = true;
			}
			catch ( VTX::ScriptException & e )
			{
				jobResult.success	= false;
				jobResult.resultStr = e.what();
				if ( not p_promise )
				{
					VTX_ERROR( "Error while running script : {}", e.what() );
				}
			}
			if ( p_promise )
			{
				p_promise->set_value( std::move( jobResult ) );
			}
		}

		std::atomic<std::chrono::milliseconds> _inactivitySleepTime { std::chrono::milliseconds( 100 ) };
		std::atomic_bool					   _threadedLoopStarted = false;
		std::optional<VTX::PythonBinding::Interpretor>
			_interpretor; // Optional because it will be created and destroyed in the python thread
		Util::DataLocker<std::queue<InterpretorInstructionsOneShot>> _lockedInstructions;
		Util::DataLocker<std::queue<WaitingPythonCommand>>			 _lockedCmdQueue;
		Util::Thread::BaseThread *									 _thread = nullptr;
		Util::Thread::StopToken										 _stopToken;
	};

	Interpretor::Interpretor() : _impl( new _Impl() ) {}

	void Interpretor::runCommand( const std::string & p_ ) noexcept { _impl->runCommand( p_ ); }

	void Interpretor::runCommand(
		const std::string &							  p_cmd,
		std::shared_ptr<std::promise<AsyncJobResult>> p_ret
	) noexcept
	{ _impl->runCommand( p_cmd, std::move( p_ret ) ); }

	void Interpretor::runScript( const FilePath & p_path ) noexcept { _impl->runScript( p_path ); }

	void Interpretor::runScript(
		const FilePath &							  p_path,
		std::shared_ptr<std::promise<AsyncJobResult>> p_future
	) noexcept
	{ _impl->runScript( p_path, std::move( p_future ) ); }

	std::string Interpretor::getRuntimePythonVersion() noexcept
	{
		auto promise = std::make_shared<std::promise<AsyncJobResult>>();
		auto future	 = promise->get_future();
		_impl->runCommand( "'{}.{}.{}'.format(*__import__(\"sys\").version_info[:3])", promise );
		const AsyncJobResult result = future.get();
		return result.resultStr.substr( 1, result.resultStr.size() - 2 );
	}

	// bool Interpretor::lastScriptFailed() const { return _impl->lastScriptFailed(); }
	void Interpretor::slowerResponseTime() noexcept { _impl->slowerResponseTime(); }

	void Interpretor::fasterResponseTime() noexcept { _impl->fasterResponseTime(); }

	void Interpretor::subscribe( InterpretorInstructionsOneShot _ ) noexcept { _impl->subscribe( std::move( _ ) ); }

	void Interpretor::Del::operator()( Interpretor::_Impl * p_ ) noexcept { delete p_; }
} // namespace VTX::App::PythonBinding
