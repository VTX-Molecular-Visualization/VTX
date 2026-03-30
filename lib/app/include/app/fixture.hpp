#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "vtx_app.hpp"
#include <mutex>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App
{
	class Fixture
	{
		inline static std::once_flag _loggerOnce;

	  public:
		inline Fixture()
		{
			Arguments args;
			args.noGraphics = true;
			args.noUpdates	= true;
			args.noGui		= true;
			args.debug		= true;
			_app			= std::make_unique<VTXApp>( std::move( args ) );
			_app->start();
		}

		inline VTXApp * const get() { return _app.get(); }
		inline ~Fixture() { _app.reset(); }

	  private:
		std::unique_ptr<VTXApp> _app;
	};
} // namespace VTX::App

#endif
