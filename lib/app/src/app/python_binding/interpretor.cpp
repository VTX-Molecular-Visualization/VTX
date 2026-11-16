#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include <atomic>
#include <condition_variable>
#include <fmt/format.h>
#include <memory>
#include <mutex>
#include <optional>
#include <pybind11/pybind11.h>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/module.hpp>
#include <queue>
#include <thread>
#include <util/datalocker.hpp>
#include <util/logger.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>

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

	class Interpretor::Impl
	{
	  public:
		Impl() :
			_thread( &THREAD().createThread(
				[ this ]( const Util::Thread::StopToken & p_stopToken, Util::Thread::BaseThread & p_thread )
				{ return runPythonThread( p_stopToken, p_thread ); }
			) )
		{
			_thread->setProgressCallback( {} );
			_thread->setTerminatedCallback( {} );
		}

		~Impl()
		{
			_threadedLoopStarted.wait( false );
			if ( _threadFinished )
			{
				return;
			}
			try
			{
				_thread->stop();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Failed to stop python thread: {}", p_e.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Failed to stop python thread: unknown exception" );
			}
			try
			{
				_thread->wait();
			}
			catch ( ... )
			{
				LOGGER::logException( "Failed to wait for python thread" );
			}
		}

		void runCommand( const std::string & p_command )
		{
			{
				auto queue = _lockedCmdQueue.open();
				if ( _failed )
				{
					VTX_ERROR( "Python interpretor is not available, command ignored: {}", p_command );
					return;
				}
				queue->push( { p_command } );
			}
			_notify();
		}

		void runCommand( const std::string & p_command, const std::shared_ptr<std::promise<AsyncJobResult>> & p_ret )
		{
			bool queued = false;
			{
				auto queue = _lockedCmdQueue.open();
				if ( not _failed )
				{
					queue->push( { p_command, p_ret } );
					queued = true;
				}
			}
			if ( queued )
			{
				_notify();
				return;
			}
			if ( p_ret )
			{
				p_ret->set_value( _unavailableResult() );
			}
		}

		void runScript( const FilePath & p_path, const std::shared_ptr<std::promise<AsyncJobResult>> & p_promise )
		{
			// This method can be called from python thread directly. In this specific scenario, we don't need to post
			// an instruction to the python thread queue.
			if ( std::this_thread::get_id() != _threadId.load() )
			{
				bool queued = false;
				{
					auto instructions = _lockedInstructions.open();
					if ( not _failed )
					{
						instructions->emplace(
							[ this, path = p_path, guard = std::make_shared<ScriptPromiseGuard>( p_promise ) ](
								VTX::PythonBinding::Interpretor &
							) { _actuallyRunScript( path, std::exchange( guard->promise, nullptr ) ); }
						);
						queued = true;
					}
				}
				if ( queued )
				{
					_notify();
					return;
				}
				VTX_ERROR( "Python interpretor is not available, script ignored: {}", p_path.string() );
				if ( p_promise )
				{
					p_promise->set_value( _unavailableResult() );
				}
			}
			else
			{
				_actuallyRunScript( p_path, p_promise );
			}
		}

		void runScript( const FilePath & p_path ) { runScript( p_path, nullptr ); }

		void post( InterpretorInstructionsOneShot p_instruction )
		{
			{
				auto instructions = _lockedInstructions.open();
				if ( _failed )
				{
					VTX_ERROR( "Python interpretor is not available, instruction ignored" );
					return;
				}
				instructions->push( std::move( p_instruction ) );
			}
			_notify();
		}

		void setOnPending( const Util::AtomicCallback::Function p_function, void * const p_context ) noexcept
		{ _onPending.set( p_function, p_context ); }

		int runPythonThread( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & )
		{
			const FinishedGuard finishedGuard( _threadFinished );
			_threadId  = std::this_thread::get_id();
			_stopToken = std::move( p_stopToken );
			try
			{
				_interpretor.emplace();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Python interpretor initialization failed: {}", p_e.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Python interpretor initialization failed: unknown exception" );
			}
			_threadedLoopStarted = true;
			_threadedLoopStarted.notify_all();

			if ( not _interpretor.has_value() )
			{
				_failPendingJobs();
				return 0;
			}

			try
			{
				this->_listenQueue();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Unhandled exception in python thread: {}", p_e.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Unhandled unknown exception in python thread" );
			}
			_failPendingJobs();
			_interpretor.reset();
			return 0;
		}

	  private:
		void _listenQueue()
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
					try
					{
						instruction( *_interpretor );
					}
					catch ( const std::exception & p_e )
					{
						VTX_ERROR( "Python instruction failed: {}", p_e.what() );
					}
					catch ( ... )
					{
						VTX_ERROR( "Python instruction failed: unknown exception" );
					}
					_onPending();
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
					_onPending();
					goto loop_beginning;
				}

				{
					std::unique_lock lock( _wakeMutex );
					_wakeCondition.wait( lock, _stopToken, [ this ]() { return _hasPendingWork(); } );
				}
				if ( _stopToken.stop_requested() )
				{
					break;
				}
			}
		}

		bool _hasPendingWork()
		{ return not _lockedInstructions.open()->empty() || not _lockedCmdQueue.open()->empty(); }

		void _notify()
		{
			{
				const std::scoped_lock lock( _wakeMutex );
			}
			_wakeCondition.notify_one();
		}

		void _actuallyRunCommand( WaitingPythonCommand & p_command )
		{
			AsyncJobResult jobResult;
			try
			{
				VTX_PYTHON_IN( "{}", p_command.commandStr );
				if ( not _interpretor.has_value() )
				{
					throw std::bad_optional_access();
				}
				jobResult.resultStr = _interpretor->runCommand( p_command.commandStr );
				jobResult.success	= true;
				if ( not jobResult.resultStr.empty() )
				{
					VTX_PYTHON_OUT( "{}", jobResult.resultStr );
				}
			}
			catch ( const CommandException & p_e )
			{
				VTX_PYTHON_OUT( "Command exception raised in python command : {}", p_e.what() );
				jobResult.resultStr = p_e.what();
			}
			catch ( const ScriptException & p_e )
			{
				VTX_PYTHON_OUT( "Script exception raised in python command : {}", p_e.what() );
				jobResult.resultStr = p_e.what();
			}
			catch ( const std::exception & p_e )
			{
				VTX_PYTHON_OUT( "Exception raised in python command : {}", p_e.what() );
				jobResult.resultStr = p_e.what();
			}
			catch ( ... )
			{
				VTX_PYTHON_OUT( "Unknown exception raised in python command" );
				jobResult.resultStr = "Unknown exception";
			}

			if ( p_command.promise )
			{
				p_command.promise->set_value( std::move( jobResult ) );
			}
		}

		void _actuallyRunScript(
			const FilePath &									  p_path,
			const std::shared_ptr<std::promise<AsyncJobResult>> & p_promise
		)
		{
			AsyncJobResult jobResult;
			try
			{
				if ( not _interpretor.has_value() )
				{
					throw std::bad_optional_access();
				}
				_interpretor->runScript( p_path );
				jobResult.success = true;
			}
			catch ( const VTX::ScriptException & e )
			{
				jobResult.success	= false;
				jobResult.resultStr = e.what();
				if ( not p_promise )
				{
					VTX_ERROR( "Error while running script : {}", e.what() );
				}
			}
			catch ( const std::exception & e )
			{
				jobResult.success	= false;
				jobResult.resultStr = e.what();
				if ( not p_promise )
				{
					VTX_ERROR( "Error while running script : {}", e.what() );
				}
			}
			catch ( ... )
			{
				jobResult.success	= false;
				jobResult.resultStr = "Unknown exception";
				if ( not p_promise )
				{
					VTX_ERROR( "Error while running script : unknown exception" );
				}
			}
			if ( p_promise )
			{
				p_promise->set_value( std::move( jobResult ) );
			}
		}

		std::atomic_bool _threadedLoopStarted = false;
		std::atomic_bool _failed			  = false;
		std::optional<VTX::PythonBinding::Interpretor>
			_interpretor; // Optional because it will be created and destroyed in the python thread
		Util::DataLocker<std::queue<InterpretorInstructionsOneShot>> _lockedInstructions;
		Util::DataLocker<std::queue<WaitingPythonCommand>>			 _lockedCmdQueue;
		std::atomic_bool											 _threadFinished = false;
		std::atomic<std::thread::id>								 _threadId;
		Util::Thread::StopToken										 _stopToken;
		Util::Thread::BaseThread *									 _thread = nullptr;
		std::mutex													 _wakeMutex;
		std::condition_variable_any									 _wakeCondition;
		Util::AtomicCallback										 _onPending;

		static AsyncJobResult _unavailableResult()
		{ return AsyncJobResult { .success = false, .resultStr = "Python interpretor is not available." }; }

		struct FinishedGuard
		{
			explicit FinishedGuard( std::atomic_bool & p_finished ) noexcept : finished( p_finished ) {}

			FinishedGuard( const FinishedGuard & )			   = delete;
			FinishedGuard & operator=( const FinishedGuard & ) = delete;

			~FinishedGuard() { finished = true; }

			std::atomic_bool & finished;
		};

		struct ScriptPromiseGuard
		{
			explicit ScriptPromiseGuard( std::shared_ptr<std::promise<AsyncJobResult>> p_promise ) noexcept :
				promise( std::move( p_promise ) )
			{
			}

			ScriptPromiseGuard( const ScriptPromiseGuard & )			 = delete;
			ScriptPromiseGuard & operator=( const ScriptPromiseGuard & ) = delete;

			~ScriptPromiseGuard()
			{
				if ( not promise )
				{
					return;
				}
				try
				{
					promise->set_value( _unavailableResult() );
				}
				catch ( ... )
				{
					LOGGER::logException( "Failed to fulfil python script promise" );
				}
			}

			std::shared_ptr<std::promise<AsyncJobResult>> promise;
		};

		void _failPendingJobs()
		{
			_failed = true;
			{
				auto instructions = _lockedInstructions.open();
				*instructions	  = {};
			}
			{
				auto queue = _lockedCmdQueue.open();
				while ( not queue->empty() )
				{
					if ( queue->front().promise )
					{
						queue->front().promise->set_value( _unavailableResult() );
					}
					queue->pop();
				}
			}
			_onPending();
		}
	};

	Interpretor::Interpretor() : _impl( new Impl() ) {}

	void Interpretor::runCommand( const std::string & p_ ) { _impl->runCommand( p_ ); }

	void Interpretor::runCommand(
		const std::string &									  p_cmd,
		const std::shared_ptr<std::promise<AsyncJobResult>> & p_ret
	)
	{ _impl->runCommand( p_cmd, p_ret ); }

	void Interpretor::runScript( const FilePath & p_path ) { _impl->runScript( p_path ); }

	void Interpretor::runScript(
		const FilePath &									  p_path,
		const std::shared_ptr<std::promise<AsyncJobResult>> & p_future
	)
	{ _impl->runScript( p_path, p_future ); }

	void Interpretor::requestRuntimePythonVersion( std::function<void( std::string )> p_callback )
	{
		_impl->post(
			[ callback = std::move( p_callback ) ]( VTX::PythonBinding::Interpretor & )
			{
				const pybind11::object versionInfo = pybind11::module_::import( "sys" ).attr( "version_info" );
				callback(
					fmt::format(
						"{}.{}.{}",
						versionInfo.attr( "major" ).cast<int>(),
						versionInfo.attr( "minor" ).cast<int>(),
						versionInfo.attr( "micro" ).cast<int>()
					)
				);
			}
		);
	}

	void Interpretor::post( InterpretorInstructionsOneShot _ ) { _impl->post( std::move( _ ) ); }

	void Interpretor::setOnPending( const Util::AtomicCallback::Function p_function, void * const p_context ) noexcept
	{ _impl->setOnPending( p_function, p_context ); }

	void Interpretor::requestFunctionList( std::function<void( std::vector<std::string> )> p_callback )
	{
		_impl->post( [ callback = std::move( p_callback ) ]( VTX::PythonBinding::Interpretor & )
					 { callback( VTX::PythonBinding::Wrapper::Module::getFunctionList() ); } );
	}

	void Interpretor::Del::operator()( Interpretor::Impl * const p_ ) noexcept
	{ std::default_delete<Interpretor::Impl> {}( p_ ); }
} // namespace VTX::App::PythonBinding
