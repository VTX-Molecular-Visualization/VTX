#ifndef __VTX_APP_OLD_APPLICATION_DEFINE__
#define __VTX_APP_OLD_APPLICATION_DEFINE__

#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Application
{
	// VTX.
	const std::string VTX_PROJECT_NAME	   = "VTX";
	constexpr uint	  VTX_VERSION_MAJOR	   = 0;
	constexpr uint	  VTX_VERSION_MINOR	   = 4;
	constexpr uint	  VTX_VERSION_REVISION = 1;

	const std::string VTX_WEBSITE_URL			= "https://vtx.drugdesign.fr";
	const std::string VTX_VERSION_URL			= VTX_WEBSITE_URL + "/vtx.json";
	const std::string VTX_WEBSITE_CLICKABLE_URL = "<a href=\"" + VTX_WEBSITE_URL + "\">" + VTX_WEBSITE_URL + "</a>";

	const std::string VTX_GIT_URL				 = "https://github.com/VTX-Molecular-Visualization/VTX";
	const std::string VTX_DOCUMENTATION_URL		 = "https://github.com/VTX-Molecular-Visualization/VTX";
	const std::string VTX_BUG_REPORT_URL		 = "https://github.com/VTX-Molecular-Visualization/VTX/issues";
	const std::string VTX_RELEASES_URL			 = "https://github.com/VTX-Molecular-Visualization/VTX/releases";
	const std::string VTX_RELEASES_CLICKABLE_URL = "<a href=\"" + VTX_RELEASES_URL + "\">" + VTX_RELEASES_URL + "</a>";

	// API.
	const std::string API_URL_MMTF = "https://mmtf.rcsb.org/v1.0/full/";

	// Register Keys
	namespace RegisterKey
	{
		const std::string RECENT_LOADED_PATH_PREFIX		= "RecentLoadedPath";
		const std::string RECENT_DOWNLOADED_CODE_PREFIX = "RecentDownloadCode";

		const std::string LAST_OPEN_SESSION_FOLDER		= "LastOpenSessionFolder";
		const std::string LAST_SAVED_SESSION_FOLDER		= "LastSavedSessionFolder";
		const std::string LAST_IMPORTED_MOLECULE_FOLDER = "LastImportedMoleculeFolder";
		const std::string LAST_EXPORTED_MOLECULE_FOLDER = "LastExportedMoleculeFolder";
		const std::string LAST_EXPORTED_IMAGE_FOLDER	= "LastExportedImageFolder";

	} // namespace RegisterKey

} // namespace VTX::App::Old::Application

#endif
