#include "app/python_binding/interpretor.hpp"
#include "app/core/threading/threading_system.hpp"
#include <python_binding/interpretor.hpp>
#include <queue>
#include <util/datalocker.hpp>

namespace VTX::App::PythonBinding
{
	class Interpretor::_Impl
	{
	  public:
		_Impl() :
			_thread( &THREADING_SYSTEM().createThread(
				[ this ]( Util::StopToken p_stopToken, App::Core::Threading::BaseThread & _ )
				{
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
			while ( true )
				std::this_thread::sleep_for()
		}
		VTX::PythonBinding::Interpretor & pythonInterpretor() { return _interpretor; }

	  private:
		VTX::PythonBinding::Interpretor			  _interpretor;
		Util::DataLocker<std::queue<std::string>> _lockedCmdQueue;
		App::Core::Threading::BaseThread *		  _thread = nullptr;
		Util::StopToken							  _stopToken;
	};

	Interpretor::Interpretor() : _impl( new _Impl() ) {}
	void Interpretor::runCommand( const std::string & p_ ) { _impl->runCommand( p_ ); }
	void Interpretor::Del::operator()( Interpretor::_Impl * p_ ) { delete p_; }
} // namespace VTX::App::PythonBinding
