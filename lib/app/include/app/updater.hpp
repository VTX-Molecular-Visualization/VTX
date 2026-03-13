#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <memory>
#include <optional>

namespace VTX::App
{

	/**
	 * @brief Application updater.
	 */
	class Updater
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Updater();
		~Updater();

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
		 * @brief PImpl.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
} // namespace VTX::App

#endif
