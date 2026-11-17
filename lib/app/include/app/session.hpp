#ifndef VTX_APP_SESSION_HPP
#define VTX_APP_SESSION_HPP

#include "app/constants.hpp"
#include "app/events.hpp"
#include <memory>
#include <optional>
#include <util/types.hpp>

namespace VTX::App
{

	/**
	 * @brief Runtime session.
	 */
	class Session
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Session();
		Session( const Session & )			   = delete;
		Session & operator=( const Session & ) = delete;
		Session( Session && )				   = delete;
		Session & operator=( Session && )	   = delete;
		~Session();

		/**
		 * @brief Current version.
		 */
		static std::string version()
		{
			return std::to_string( VERSION_MAJOR ) + "." + std::to_string( VERSION_MINOR ) + "."
				   + std::to_string( VERSION_PATCH );
		}

		/**
		 * @brief Run Velopack startup hooks before application argument parsing.
		 */
		static void handleStartupActivation();

		/**
		 * @brief Check for application update.
		 */
		void checkForUpdate();

		/**
		 * @brief Download the latest update.
		 */
		void downloadUpdate();

		/**
		 * @brief Apply a previously downloaded update and restart the application.
		 */
		void applyDownloadedUpdate();

		/**
		 * @brief Does the application have an update manager available (not in dev mode).
		 */
		bool hasManager() const noexcept;

		/**
		 * @brief Is this version portable.
		 */
		bool isPortable() const;

		/**
		 * @brief Get application directories/files.
		 */
		FilePath getDataHome() const;
		FilePath getPicturesFolder() const;

		FilePath getApplicationDir() const;
		FilePath getAppTmpFolder() const;
		FilePath getLicenseFile() const;
		FilePath getDataDir() const;
		FilePath getDefaultUserData() const;
		FilePath getDefaultRepresentationsDir() const;
		FilePath getDefaultColorLayoutsDir() const;
		FilePath getDefaultEffectsDir() const;
		FilePath getDefaultHdriDir() const;
		FilePath getDefaultMaterialsDir() const;
		FilePath getDefaultScriptsDir() const;
		FilePath getLogsDir() const;
		FilePath getCacheDir() const;
		FilePath getSnapshotsDir() const;
		FilePath getRepresentationsDir() const;
		FilePath getColorLayoutsDir() const;
		FilePath getEffectsDir() const;
		FilePath getHdriDir() const;
		FilePath getMaterialsDir() const;
		FilePath getScriptsDir() const;
		FilePath getConfigIniFile() const;

		/**
		 * @brief Print session information (for debug).
		 */
		void print() const;

	  private:
		/**
		 * @brief PImpl.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;

		/**
		 * @brief Called each frame from the main thread to dispatch the update check result.
		 */
		void _onUpdateCheckResult( const Events::Update & );

		/**
		 * @brief Called each frame from the main thread to dispatch the update download result.
		 */
		void _onUpdateDownloadResult( const Events::Update & );

		/**
		 * @brief Called each frame from the main thread to dispatch update download progress.
		 */
		void _onUpdateDownloadProgress( const Events::Update & );
	};
} // namespace VTX::App

#endif
