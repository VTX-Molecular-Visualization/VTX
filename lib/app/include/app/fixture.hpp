#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App
{
	class Fixture
	{
	  public:
		inline Fixture()
		{
			VTX::VTX_INFO( "Constructing fixture" );
			if ( not _app )
			{
				VTX::VTX_INFO( "Fixture constructing app" );
				Args args( { ARG_NO_GRAPHICS, ARG_NO_UPDATE } );
				_app = std::make_unique<APP>( args );
				VTX::VTX_INFO( "Fixture initializing app" );
				APP::init();
			}
			else
				VTX::VTX_INFO( "App already constructed." );

			VTX::VTX_INFO( "Fixture starting app" );
			_app->start();
		}

		inline ~Fixture() { _app->stop(); }

	  private:
		inline static std::unique_ptr<APP> _app;
	};
} // namespace VTX::App

#endif
