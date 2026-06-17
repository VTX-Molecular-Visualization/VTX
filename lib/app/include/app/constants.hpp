#ifndef __VTX_APP_CONSTANTS__
#define __VTX_APP_CONSTANTS__

#include <string>
#include <string_view>
#include <util/types.hpp>

namespace VTX::App
{
	// Resolved in .cpp with preprocessor definitions only passed to App.
	extern const uint VERSION_MAJOR;
	extern const uint VERSION_MINOR;
	extern const uint VERSION_PATCH;

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX Team";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";

	constexpr std::string_view URL_UPDATE	   = "https://github.com/VTX-Molecular-Visualization/VTX";
	constexpr std::string_view APP_FOLDER_NAME = "VTX";

	constexpr std::string_view URL_WEBSITE		 = "https://vtx.drugdesign.fr";
	constexpr std::string_view URL_DOCUMENTATION = "https://vtx.drugdesign.fr/doc";
	constexpr std::string_view URL_REPORT		 = "https://github.com/VTX-Molecular-Visualization/VTX/issues";

	// TODO: move.
	constexpr std::string_view LOG_LINK_REGEX  = R"(//file=\[([^\]]+)\])";
	constexpr std::string_view LOG_LINK_FORMAT = "//file=[{}]";

} // namespace VTX::App

#endif
