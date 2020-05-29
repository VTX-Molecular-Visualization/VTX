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
				const float newNear = Util::Math::clamp( _near, CAMERA_NEAR_FAR_MIN, CAMERA_NEAR_FAR_MAX );
				const float newFar	= Util::Math::clamp( _far, CAMERA_NEAR_FAR_MIN, CAMERA_NEAR_FAR_MAX );

				Setting::Rendering::camNear = Util::Math::min( newNear, newFar );
				Setting::Rendering::camFar	= Util::Math::max( newNear, newFar );

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
