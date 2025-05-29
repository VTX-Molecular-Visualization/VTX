#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "vtx_app.hpp"

namespace VTX::App
{
	class Fixture
	{
	  public:
		Fixture()
		{
			if ( not _app )
			{
				_app = std::make_unique<APP>();
				APP::init();
			}

			Args args( { ARG_NO_GRAPHICS, ARG_NO_UPDATE } );
			_app->start( args );
		}

		~Fixture() { _app->stop(); }

	  private:
		inline static std::unique_ptr<APP> _app;
	};
} // namespace VTX::App

#endif
