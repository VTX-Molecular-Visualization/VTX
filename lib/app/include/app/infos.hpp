#ifndef __VTX_APP_INFO__
#define __VTX_APP_INFO__

#include <string>

namespace VTX::App::Info
{
#ifndef VTX_VERSION_MAJOR
	constexpr int VERSION_MAJOR = 0;
#else
	constexpr int VERSION_MAJOR = VTX_VERSION_MAJOR;
#endif
#ifndef VTX_VERSION_MINOR
	constexpr int VERSION_MINOR = 0;
#else
	constexpr int VERSION_MINOR = VTX_VERSION_MINOR;
#endif
#ifndef VTX_VERSION_REVISION
	constexpr int VERSION_REVISION = 0;
#else
	constexpr int VERSION_REVISION = VTX_VERSION_REVISION;
#endif

	constexpr std::string_view APPLICATION_DISPLAY_NAME = "VTX";
	constexpr std::string_view APPLICATION_NAME			= "VTX";
	constexpr std::string_view ORGANIZATION_NAME		= "VTX";
	constexpr std::string_view ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";

} // namespace VTX::App::Info

#endif
