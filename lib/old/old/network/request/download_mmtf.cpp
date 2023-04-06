#include "download_mmtf.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "define.hpp"

namespace VTX::Network::Request
{
	DownloadMMTF::DownloadMMTF( const std::string & p_id ) :
		NetworkRequest( std::string( API_URL_MMTF + p_id ) ), _id( p_id )
	{
		VTX_INFO( "Downloading " + p_id + "..." );
	}

	void DownloadMMTF::_success( QNetworkReply * const p_reply )
	{
		VTX_INFO( "Downloaded" );
		std::map<FilePath, std::string *> mapBuffers = std::map<FilePath, std::string *>();
		mapBuffers.emplace( FilePath( _id + ".mmtf" ), new std::string( p_reply->readAll().toStdString() ) );

		VTX_ACTION( new Action::Main::Open( mapBuffers ) );
	}

} // namespace VTX::Network::Request
