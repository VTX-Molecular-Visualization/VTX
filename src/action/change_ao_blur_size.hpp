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
		class ChangeAOBlurSize : public BaseAction
		{
		  public:
			explicit ChangeAOBlurSize( const int p_blurSize ) : _blurSize( p_blurSize ) {}

			virtual void execute() override
			{
				Setting::Rendering::aoBlurSize
					= Util::Math::clamp( _blurSize, RENDERER_AO_BLUR_SIZE_MIN, RENDERER_AO_BLUR_SIZE_MAX );
			};

		  private:
			const int _blurSize;
		};
	} // namespace Action
} // namespace VTX
#endif
