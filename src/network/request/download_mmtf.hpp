#ifndef __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__
#define __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__

#include "network/network_request.hpp"

namespace VTX::Network::Request
{
	class DownloadMMTF : public NetworkRequest
	{
		VTX_REQUEST

	  public:
		DownloadMMTF( const std::string & p_id );

	  private:
		std::string _id;

		void _success( QNetworkReply * const p_reply ) override;
	};
} // namespace VTX::Network::Request
#endif
