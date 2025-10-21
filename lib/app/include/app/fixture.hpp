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
				Args args( { ARG_NO_GRAPHICS, ARG_NO_UPDATE } );
				_app = std::make_unique<VTXApp>( args );
			}

			_app->start();
		}

		~Fixture() = default;

		inline VTXApp * const get() { return _app.get(); }

	  private:
		inline static std::unique_ptr<VTXApp> _app;
	};
} // namespace VTX::App

#endif
