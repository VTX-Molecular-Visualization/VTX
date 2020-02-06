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
		class ActionChangeShading : public BaseAction
		{
		  public:
			explicit ActionChangeShading( const Renderer::SHADING p_shading ) : _shading( p_shading ) {}

			virtual void execute() override
			{
				Setting::Rendering::shading = _shading;
				VTXApp::get().getRenderer().setShading();
			};

		  private:
			const Renderer::SHADING _shading;
		};
	} // namespace Action
} // namespace VTX
#endif
