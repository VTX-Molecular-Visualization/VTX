#ifndef __VTX_ACTION_CHANGE_AO_BLUR_SIZE__
#define __VTX_ACTION_CHANGE_AO_BLUR_SIZE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeAOBlurSize : public BaseAction
		{
		  public:
			explicit ActionChangeAOBlurSize( const int p_blurSize ) : _blurSize( p_blurSize ) {}

			virtual void execute() override
			{
				Setting::Rendering::aoBlurSize
					= glm::clamp( _blurSize, RENDERER_AO_BLUR_SIZE_MIN, RENDERER_AO_BLUR_SIZE_MAX );
			};

		  private:
			const int _blurSize;
		};
	} // namespace Action
} // namespace VTX
#endif
