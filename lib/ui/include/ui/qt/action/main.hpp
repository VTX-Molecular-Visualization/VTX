#ifndef __VTX_UI_QT_ACTION_APPLICATION__
#define __VTX_UI_QT_ACTION_APPLICATION__

#include <app/core/action/base_action.hpp>
#include <app/old_app/id.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Action::Main
{
	class Quit : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit Quit() {}
		virtual void execute() override;
	};

	class RestoreLayout : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit RestoreLayout() {}
		virtual void execute() override;
	};

	class ToggleCameraController : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit ToggleCameraController() {}
		virtual void execute() override;
	};

	class ChangeCameraController : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}
		virtual void execute() override;

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit ResetCameraController() {}
		virtual void execute() override;
	};

	class CheckForUpdate : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit CheckForUpdate( const bool p_showPopupIfNoUpdate = false ) :
			_showPopupIfNoUpdate( p_showPopupIfNoUpdate )
		{
		}

		virtual void execute() override;
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		bool _showPopupIfNoUpdate;
	};
} // namespace VTX::UI::QT::Action::Main
#endif
