#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include <app/core/action/base_action.hpp>

namespace VTX::App::Action::Camera
{

	class SetCameraProjectionOrthographic final : public Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionOrthographic() {}
		void execute() override;
	};

	class SetCameraProjectionPerspective final : public Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionPerspective() {}
		void execute() override;
	};

	class ToggleCameraProjection final : public Core::Action::BaseAction
	{
	  public:
		ToggleCameraProjection() {}
		void execute() override;
	};

	class Reset final : public Core::Action::BaseAction
	{
	  public:
		Reset() {}
		void execute() override;
	};
} // namespace VTX::App::Action::Camera
#endif
