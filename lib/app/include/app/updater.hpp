#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <util/json/json.hpp>
#include <util/network.hpp>

namespace VTX::App
{
	// TODO: move to a dedicated file with other constants/API.
	constexpr std::string_view UPDATER_URL
		= "https://api.github.com/repos/VTX-Molecular-Visualization/VTX/releases/latest";

	class Updater
	{
	  public:
		void checkForUpdate();
		void downloadUpdate();

		Util::Callback<uint, uint, uint> onUpdateAvailable;

	  private:
		Util::JSon::Document _document;
	};
} // namespace VTX::App

#endif
