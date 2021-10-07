#ifndef __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__
#define __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__

#include "network/network_request.hpp"

namespace VTX::Network::Request
{
	class DownloadMMTF : public NetworkRequest
	{
	  public:
		DownloadMMTF( const std::string & p_id );
		void success( QNetworkReply * const p_reply ) override;

	  private:
		std::string _id;
	};
} // namespace VTX::Network::Request
#endif
