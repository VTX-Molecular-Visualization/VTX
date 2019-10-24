#ifndef __VTX_APP__
#define __VTX_APP__

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

	  private:
		UI::UserInterface _ui		 = UI::UserInterface();
		bool			  _isRunning = false;
		double			  _time		 = 0.;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};
} // namespace VTX

#endif
