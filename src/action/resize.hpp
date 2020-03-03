#ifndef __VTX_ACTION_RESIZE__
#define __VTX_ACTION_RESIZE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Resize : public BaseAction
		{
		  public:
			Resize( const uint p_width, const uint p_height ) : _width( p_width ), _height( p_height ) {}
			virtual void execute() override
			{
				// Set camera.
				VTXApp::get().getScene().getCamera().setScreenSize( _width, _height );

				// Resize renderer.
				VTXApp::get().getRenderer().resize( _width, _height );
			};

		  private:
			uint _width;
			uint _height;
		};
	} // namespace Action
} // namespace VTX
#endif
