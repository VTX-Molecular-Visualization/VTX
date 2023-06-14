#ifndef __VTX_CONTROLLER_MAIN_WINDOW__
#define __VTX_CONTROLLER_MAIN_WINDOW__

#include "base_keyboard_controller.hpp"
#include "ui/id.hpp"
#include <app/old/id.hpp>

namespace VTX::UI::QT::Controller
{
	class MainWindowController : public BaseKeyboardController
	{
	  public:
		MainWindowController() : BaseKeyboardController() {}

		inline VTX::App::Old::VTX_ID getTargetWidget() override { return UI::ID::Input::MAIN_WINDOW; }
		inline bool				readEventFromInput( const VTX::App::Old::VTX_ID & p_id ) override
		{
			// This controller events from every target
			return true;
		};

		void		 update( const float & p_deltaTime ) override {};
		virtual void receiveEvent( const QKeyEvent & p_event ) override;
	};
} // namespace VTX::UI::QT::Controller
#endif
