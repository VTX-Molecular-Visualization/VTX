#ifndef __VTX_APP__
#define __VTX_APP__

#include "event/event_ui.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"

namespace VTX
{
	class VTXApp final
	{
	  public:
		static VTXApp & INSTANCE()
		{
			static VTXApp instance;
			return instance;
		}

		void start();
		void stop();

		void fireUIEvent( Event::EVENT_UI, va_list );

	  private:
		UI::UserInterface * _ui		   = nullptr;
		bool				_isRunning = false;
		double				_time	   = 0.;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};
} // namespace VTX

#endif
