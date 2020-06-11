#ifndef __VTX_ACTION_CHANGE_CAMERA_CLIP__
#define __VTX_ACTION_CHANGE_CAMERA_CLIP__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeCameraClip : public BaseAction
		{
		  public:
			explicit ChangeCameraClip( const float p_near, const float p_far ) : _near( p_near ), _far( p_far ) {}

			virtual void execute() override
			{
				Setting::Rendering::camNear = Util::Math::min( _near, _far );
				Setting::Rendering::camFar	= Util::Math::max( _near, _far );

				VTXApp::get().getScene().getCamera().setNear( Setting::Rendering::camNear );
				VTXApp::get().getScene().getCamera().setFar( Setting::Rendering::camFar );
			};

		  private:
			const float _near;
			const float _far;
		};
	} // namespace Action
} // namespace VTX
#endif
