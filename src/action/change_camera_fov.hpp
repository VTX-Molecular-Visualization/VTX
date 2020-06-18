#ifndef __VTX_ACTION_CHANGE_CAMERA_FOV__
#define __VTX_ACTION_CHANGE_CAMERA_FOV__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeCameraFov : public BaseAction
		{
		  public:
			explicit ChangeCameraFov( const float p_fov ) : _fov( p_fov ) {}

			virtual void execute() override
			{
				VTX_SETTING().cameraFov = _fov;

				VTXApp::get().getScene().getCamera().setFov( VTX_SETTING().cameraFov );
			};

		  private:
			const float _fov;
		};
	} // namespace Action
} // namespace VTX
#endif
