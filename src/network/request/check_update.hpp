#ifndef __VTX_NETWORK_REQUEST_CHECK_UPDATE__
#define __VTX_NETWORK_REQUEST_CHECK_UPDATE__

#include "network/network_request.hpp"

namespace VTX::Network::Request
{
	class CheckUpdate : public NetworkRequest
	{
	  public:
		CheckUpdate();
		void success( QNetworkReply * const p_reply ) override;
	};

} // namespace VTX::Network::Request
#endif
