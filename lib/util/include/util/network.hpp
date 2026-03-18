#ifndef __VTX_UTIL_NETWORK__
#define __VTX_UTIL_NETWORK__

#include "exceptions.hpp"
#include "logger.hpp"

namespace VTX::Util::Network
{
	using CallbackHttpGet	= std::function<void( std::string && )>;
	// Return false to cancel the download.
	using ProgressCallback	= std::function<bool( size_t p_downloaded, size_t p_total )>;

	void httpRequestGet(
		const std::string_view & p_url,
		std::string &			 p_text,
		const ProgressCallback & p_progress = nullptr
	);

} // namespace VTX::Util::Network

#endif
