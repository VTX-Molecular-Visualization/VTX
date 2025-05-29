#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <util/network.hpp>

namespace VTX::App
{
	class Updater
	{
	  public:
		void checkForUpdate();

		Util::Callback<> onUpdateAvailable;
	};
} // namespace VTX::App

#endif
