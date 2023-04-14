#ifndef __VTX_NETWORK_REQUEST_CHECK_UPDATE__
#define __VTX_NETWORK_REQUEST_CHECK_UPDATE__

#include <app/old_app/network/network_request.hpp>

namespace VTX::Network::Request
{
	class CheckUpdate : public NetworkRequest
	{
		VTX_REQUEST

	  public:
		CheckUpdate( const bool p_showPopupIfNoUpdate );

	  private:
		bool _showPopupIfNoUpdate;
		void _success( NetworkReply * const p_reply ) override;
	};

} // namespace VTX::Network::Request
#endif
