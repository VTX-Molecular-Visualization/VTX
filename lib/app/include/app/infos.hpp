#ifndef __VTX_APP_INFO__
#define __VTX_APP_INFO__

#include <string>

namespace VTX::App::Info
{
	// TODO: propagate from CMake?
	constexpr int VERSION_MAJOR	   = 1;
	constexpr int VERSION_MINOR	   = 0;
	constexpr int VERSION_REVISION = 0;

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";
	constexpr std::string_view APPLICATION_VERSION		= "1.0.0";

} // namespace VTX::App::Info

#endif
