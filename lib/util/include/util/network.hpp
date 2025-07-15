#ifndef __VTX_UTIL_NETWORK__
#define __VTX_UTIL_NETWORK__

#include "exceptions.hpp"
#include "logger.hpp"

namespace VTX::Util::Network
{
	using CallbackHttpGet = std::function<void( const std::string & )>;

	void httpRequestGet( const std::string_view & p_url, std::string & p_text );
	void httpRequestGetAsync( const std::string_view & p_url, const CallbackHttpGet & p_callback );

} // namespace VTX::Util::Network

#endif
