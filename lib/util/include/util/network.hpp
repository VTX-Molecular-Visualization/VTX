#ifndef __VTX_UTIL_NETWORK__
#define __VTX_UTIL_NETWORK__

#include "exceptions.hpp"
#include "logger.hpp"
#include <cpr/cpr.h>

namespace VTX::Util::Network
{
	using CallbackHttpGet = std::function<void( const std::string & )>;

	void httpRequestGet( const std::string & p_url, std::string & p_data );

} // namespace VTX::Util::Network

#endif
