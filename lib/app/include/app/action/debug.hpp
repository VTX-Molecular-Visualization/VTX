#ifndef __VTX_APP_ACTION_DEBUG__
#define __VTX_APP_ACTION_DEBUG__

#include "app/action/base_action.hpp"

namespace VTX::App::Action::Debug
{
	class ToggleRendererActivation final : public BaseAction
	{
	  public:
		void execute() override;
	};

	class RefreshSES final : public BaseAction
	{
	  public:
		void execute() override;
	};

	class ChangeRenderMode final : public BaseAction
	{
	  public:
		void execute() override;
	};

	class PrintCameraInfo final : public BaseAction
	{
	  public:
		void execute() override;
	};

} // namespace VTX::App::Action::Debug

#endif
