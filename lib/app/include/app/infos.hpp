#ifndef __VTX_APP_INFO__
#define __VTX_APP_INFO__

#include <string>
#include <util/types.hpp>

namespace VTX::App
{
	// Resolved in .cpp with preprocessor definitions only passed to App.
	extern const uint VERSION_MAJOR;
	extern const uint VERSION_MINOR;
	extern const uint VERSION_PATCH;

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";

	constexpr std::string_view UPDATER_URL
		= "https://github.com/VTX-Molecular-Visualization/VTX/releases/latest/download";
} // namespace VTX::App

#endif
