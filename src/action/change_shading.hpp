#ifndef __VTX_ACTION_CHANGE_SHADING__
#define __VTX_ACTION_CHANGE_SHADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeShading : public BaseAction
		{
		  public:
			ChangeShading( const Renderer::SHADING p_shading ) : _shading( p_shading ) {}

			virtual void execute() override
			{
				VTX_SETTING().shading = _shading;
				VTXApp::get().getRendererGL().setShading();
			};

			virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

		  private:
			const Renderer::SHADING _shading;
		};
	} // namespace Action
} // namespace VTX
#endif
