#ifndef __VTX_DEFINE__
#define __VTX_DEFINE__

#include <string>
#include <util/types.hpp>

namespace VTX
{
	// VTX.
	const std::string VTX_PROJECT_NAME	   = "VTX";
	constexpr uint	  VTX_VERSION_MAJOR	   = 0;
	constexpr uint	  VTX_VERSION_MINOR	   = 4;
	constexpr uint	  VTX_VERSION_REVISION = 0;

	const std::string VTX_WEBSITE_URL			= "https://vtx.drugdesign.fr";
	const std::string VTX_VERSION_URL			= VTX_WEBSITE_URL + "/vtx.json";
	const std::string VTX_WEBSITE_CLICKABLE_URL = "<a href=\"" + VTX_WEBSITE_URL + "\">" + VTX_WEBSITE_URL + "</a>";

	const std::string VTX_GIT_URL				 = "https://gitlab.com/VTX_mol/VTX";
	const std::string VTX_DOCUMENTATION_URL		 = "https://gitlab.com/VTX_mol/VTX";
	const std::string VTX_BUG_REPORT_URL		 = "https://gitlab.com/VTX_mol/VTX/-/issues";
	const std::string VTX_RELEASES_URL			 = "https://gitlab.com/VTX_mol/VTX/-/releases";
	const std::string VTX_RELEASES_CLICKABLE_URL = "<a href=\"" + VTX_RELEASES_URL + "\">" + VTX_RELEASES_URL + "</a>";

	// API.
	const std::string API_URL_MMTF = "https://mmtf.rcsb.org/v1.0/full/";

	// Constants.
	constexpr Vec3f CAMERA_RIGHT_DEFAULT = VEC3F_X;
	constexpr Vec3f CAMERA_UP_DEFAULT	 = VEC3F_Y;
	constexpr Vec3f CAMERA_FRONT_DEFAULT = Vec3f( 0.f, 0.f, -1.f );

	constexpr uint INVALID_ID = UINT_MAX;

	// Mask.
	using VTX_MASK = unsigned char;

	constexpr VTX_MASK VTX_MASK_NEED_UPDATE		  = 0xFF;
	constexpr VTX_MASK VTX_MASK_NO_UPDATE		  = 0x00;
	constexpr VTX_MASK VTX_MASK_SELECTION_UPDATED = 0x01;
	constexpr VTX_MASK VTX_MASK_CAMERA_UPDATED	  = 0x02;
	constexpr VTX_MASK VTX_MASK_3D_MODEL_UPDATED  = 0x04;
	constexpr VTX_MASK VTX_MASK_UNIFORM_UPDATED	  = 0x08;
	constexpr VTX_MASK VTX_MASK_FORCE_AA		  = 0x10;

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

} // namespace VTX

#endif
