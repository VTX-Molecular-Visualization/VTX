#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App
{
	class Fixture
	{
	  public:
		Fixture()
		{
			std::call_once(
				_loggerOnce, []() { LOGGER::init( Util::Filesystem::getExecutableDir() / "logs_tests", true ); }
			);
				VTX::VTX_INFO( "Fixture constructing app" );
			else
				VTX::VTX_INFO( "App already constructed." );

			Args args( { ARG_NO_GRAPHICS, ARG_NO_UPDATE, ARG_DEBUG } );
			_app = std::make_unique<VTXApp>( args );
			_app->start();
		}

		inline VTXApp * const get() { return _app.get(); }
		{
			VTX_INFO( "Fixture deconstructor begin." );
			_app->stop();
			VTX_INFO( "Fixture deconstructor end." );
			VTX::Util::Logger::stop();
		}

	  private:
		inline static std::once_flag _loggerOnce;
		std::unique_ptr<VTXApp>		 _app;
	};
} // namespace VTX::App

#endif
