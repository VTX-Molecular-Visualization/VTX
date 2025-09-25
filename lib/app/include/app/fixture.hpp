#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "vtx_app.hpp"

namespace VTX::App
{
	class Fixture
	{
	  public:
		inline Fixture()
		{
			if ( not _app )
			{
				Args args( { ARG_NO_GRAPHICS, ARG_NO_UPDATE } );
				_app = std::make_unique<APP>( args );
				_app->init();
			}

			_app->start();
		}

		inline ~Fixture() { _app->stop(); }

	  private:
		inline static std::unique_ptr<APP> _app;
	};
} // namespace VTX::App

#endif
