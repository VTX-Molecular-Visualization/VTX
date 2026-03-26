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
			Args args( { ARG_NO_GRAPHICS, ARG_NO_GUI, ARG_NO_UPDATE, ARG_DEBUG } );
			_app = std::make_unique<VTXApp>( args );
			_app->start();
		}

		inline VTXApp * const get() { return _app.get(); }
		inline ~Fixture() { _app.reset(); }

	  private:
		std::unique_ptr<VTXApp> _app;
	};
} // namespace VTX::App

#endif
