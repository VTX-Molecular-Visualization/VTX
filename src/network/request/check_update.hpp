#ifndef __VTX_NETWORK_REQUEST_CHECK_UPDATE__
#define __VTX_NETWORK_REQUEST_CHECK_UPDATE__

#include "network/network_request.hpp"

namespace VTX::Network::Request
{
	class CheckUpdate : public NetworkRequest
	{
		VTX_REQUEST

	  public:
		CheckUpdate( const bool p_showPopupIfNoUpdate );

		std::string getStatusBarMessage() const override { return "Checking for updates..."; }

	  private:
		bool _showPopupIfNoUpdate;
		void _success( QNetworkReply * const p_reply ) override;
	};

} // namespace VTX::Network::Request
#endif
