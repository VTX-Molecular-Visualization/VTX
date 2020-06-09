#ifndef __VTX_ACTION_CHANGE_CAMERA_PROJECTION__
#define __VTX_ACTION_CHANGE_CAMERA_PROJECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeCameraProjection : public BaseAction
		{
		  public:
			explicit ChangeCameraProjection( const bool p_perspective ) : _perspective( p_perspective ) {}

			virtual void execute() override
			{
				Setting::Rendering::camPerspective = _perspective;

				VTXApp::get().getScene().getCamera().setPerspective( Setting::Rendering::camPerspective );
			};

		  private:
			const bool _perspective;
		};
	} // namespace Action
} // namespace VTX
#endif
