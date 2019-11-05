#ifndef __VTX_APP__
#define __VTX_APP__

#include "event/event.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"

namespace VTX
{
	class VTXApp final
	{
	  public:
		static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}
		static bool isRunning() { return VTXApp::_isRunning; }

		void start();
		void stop();

		template<typename T>
		void fireUIEvent( const Event::EVENT_UI,
						  const Event::Event<T> & ) const;

	  private:
		static bool			_isRunning;
		UI::UserInterface * _ui	  = nullptr;
		double				_time = 0.;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};
} // namespace VTX

#endif
