#ifndef __VTX_APP_INFO__
#define __VTX_APP_INFO__

#include <string>

namespace VTX::App::Info
{
	constexpr int VERSION_MAJOR	   = 1;
	constexpr int VERSION_MINOR	   = 0;
	constexpr int VERSION_REVISION = 0;

	static std::string APPLICATION_DISPLAY_NAME = "VTX";
	static std::string APPLICATION_NAME			= "VTX";
	static std::string ORGANIZATION_NAME		= "VTX";
	static std::string ORGANIZATION_DOMAIN		= "vtx.drugdesign.fr";
	static std::string APPLICATION_VERSION		= "1.0.0";

} // namespace VTX::App::Info

#endif
