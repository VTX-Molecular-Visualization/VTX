#include "app/old/internal/network/request/download_mmtf.hpp"
#include "app/old/action/main.hpp"
#include "app/old/application/define.hpp"
#include "app/old/manager/action_manager.hpp"

namespace VTX::App::Old::Internal::Network::Request
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

		VTX_ACTION<App::Old::Action::Main::Open>( mapBuffers );
	}

} // namespace VTX::App::Old::Internal::Network::Request
