#ifndef __VTX_BASE_RUNNABLE__
#define __VTX_BASE_RUNNABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <functional>
#include <thread>

namespace VTX
{
	namespace Generic
	{
		// TODO: rethink all that.
		class BaseRunnable
		{
		  public:
			virtual void run( const std::function<void( void )> & p_callback )
			{
				std::exception_ptr threadException = nullptr;

				// Run thread.
				std::thread thread = std::thread( [ & ]() {
					try
					{
						_run();
					}
					catch ( const std::exception & p_e )
					{
						threadException = std::current_exception();
					}
				} );

				thread.join();

				// Check exceptions.
				if ( threadException ) { std::rethrow_exception( threadException ); }

				// Call the callback.
				p_callback();
			};

		  protected:
			virtual void _run() = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
