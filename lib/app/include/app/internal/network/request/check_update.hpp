#ifndef __VTX_NETWORK_REQUEST_CHECK_UPDATE__
#define __VTX_NETWORK_REQUEST_CHECK_UPDATE__

#include "app/core/network/network_request.hpp"

namespace VTX::App::Internal::Network::Request
{
	class CheckUpdate : public Core::Network::NetworkRequest
	{
		VTX_REQUEST

	  public:
		CheckUpdate( const bool p_showPopupIfNoUpdate );

	  private:
		bool _showPopupIfNoUpdate;
		void _success( Core::Network::NetworkReply * const p_reply ) override;
	};

} // namespace VTX::App::Internal::Network::Request
#endif
