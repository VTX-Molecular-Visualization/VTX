#include "app/old_app/network/request/download_mmtf.hpp"
#include "app/old_app/action/action_manager.hpp"
#include "app/old_app/action/main.hpp"
#include "app/old_app/define.hpp"

namespace VTX::Network::Request
{
	DownloadMMTF::DownloadMMTF( const std::string & p_id ) :
		NetworkRequest( std::string( API_URL_MMTF + p_id ) ), _id( p_id )
	{
		VTX_INFO( "Downloading " + p_id + "..." );
	}

	void DownloadMMTF::_success( NetworkReply * const p_reply )
	{
		VTX_INFO( "Downloaded" );
		std::map<FilePath, std::string *> mapBuffers = std::map<FilePath, std::string *>();
		mapBuffers.emplace( FilePath( _id + ".mmtf" ), new std::string( p_reply->readAll() ) );

		VTX_ACTION( new Action::Main::Open( mapBuffers ) );
	}

} // namespace VTX::Network::Request
