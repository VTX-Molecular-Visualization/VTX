#ifndef __VTX_ACTION_CHANGE_AO_BLUR_SHARPNESS__
#define __VTX_ACTION_CHANGE_AO_BLUR_SHARPNESS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAOBlurSharpness : public BaseAction
		{
		  public:
			explicit ChangeAOBlurSharpness( const int p_blurSharpness ) : _blurSharpness( p_blurSharpness ) {}

			virtual void execute() override
			{
				Setting::Rendering::aoBlurSharpness
					= Util::Math::clamp( _blurSharpness, RENDERER_AO_BLUR_SIZE_MIN, RENDERER_AO_BLUR_SIZE_MAX );
			};

		  private:
			const int _blurSharpness;
		};
	} // namespace Action
} // namespace VTX
#endif
