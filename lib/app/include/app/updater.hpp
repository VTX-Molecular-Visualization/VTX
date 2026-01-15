#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <util/json/json.hpp>
#include <util/network.hpp>

namespace VTX::App
{

	/**
	 * @brief Application updater.
	 */
	class Updater
	{
	  public:
		/**
		 * @brief Check for application update.
		 */
		void checkForUpdate();

		/**
		 * @brief Download the latest update.
		 */
		void downloadUpdate();

	  private:
		/**
		 * @brief Distant JSON document containing release information.
		 */
		Util::JSon::Document _document;
	};
} // namespace VTX::App

#endif
