#ifndef __VTX_NETWORK_REQUEST_CHECK_UPDATE__
#define __VTX_NETWORK_REQUEST_CHECK_UPDATE__

#include <app/old/core/network/network_request.hpp>

namespace VTX::Network::Request
{
	class CheckUpdate : public App::Old::Core::Network::NetworkRequest
	{
		VTX_REQUEST

	  public:
		CheckUpdate( const bool p_showPopupIfNoUpdate );

	  private:
		bool _showPopupIfNoUpdate;
		void _success( App::Old::Core::Network::NetworkReply * const p_reply ) override;
	};

} // namespace VTX::Network::Request
#endif
