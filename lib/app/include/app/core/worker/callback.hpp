#ifndef __VTX_APP_CORE_WORKER_CALLBACK__
#define __VTX_APP_CORE_WORKER_CALLBACK__

// TODO reimplemente that without Qt and check thread which inherts from BaseThread
// #include <QThread>
#include <exception>
#include <functional>
#include <util/types.hpp>

namespace VTX::App::Core::Worker
{
	using CallbackThread = std::function<void( uint )>;
	using CallbackWorker = std::function<void( void )>;
	using CallbackError	 = std::function<void( const std::exception & )>;
} // namespace VTX::App::Core::Worker

#endif
