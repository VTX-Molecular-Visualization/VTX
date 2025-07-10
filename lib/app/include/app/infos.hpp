#ifndef __VTX_APP_INFO__
#define __VTX_APP_INFO__

#include <string>

namespace VTX::App
{
	// Resolved in .cpp with preprocessor definitions only passed to App.
	extern const int VERSION_MAJOR;
	extern const int VERSION_MINOR;
	extern const int VERSION_PATCH;

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";

} // namespace VTX::App

#endif
