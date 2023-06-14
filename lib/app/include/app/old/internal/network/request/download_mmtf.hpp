#ifndef __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__
#define __VTX_NETWORK_REQUEST_DOWNLOAD_MMTF__

#include "app/old/core/network/network_request.hpp"
#include <string>

namespace VTX::App::Old::Internal::Network::Request
{
	class DownloadMMTF : public Core::Network::NetworkRequest
	{
		VTX_REQUEST

	  public:
		DownloadMMTF( const std::string & p_id );

	  private:
		std::string _id;

		void _success( Core::Network::NetworkReply * const p_reply ) override;
	};
} // namespace VTX::App::Old::Internal::Network::Request
#endif
