#include "app/internal/network/request/download_mmtf.hpp"
#include "app/action/main.hpp"
#include "app/application/define.hpp"
#include "app/manager/action_manager.hpp"

namespace VTX::App::Internal::Network::Request
{
	DownloadMMTF::DownloadMMTF( const std::string & p_id ) :
		NetworkRequest( std::string( Application::API_URL_MMTF + p_id ) ), _id( p_id )
	{
		VTX_INFO( "Downloading {}...", p_id );
	}

	void DownloadMMTF::_success( Core::Network::NetworkReply * const p_reply )
	{
		VTX_INFO( "Downloaded" );
		std::map<FilePath, std::string *> mapBuffers = std::map<FilePath, std::string *>();
		mapBuffers.emplace( FilePath( _id + ".mmtf" ), new std::string( p_reply->readAll() ) );

		VTX_ACTION<App::Action::Main::Open>( mapBuffers );
	}

} // namespace VTX::App::Internal::Network::Request
