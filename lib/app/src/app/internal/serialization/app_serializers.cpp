#include "app/application/settings.hpp"
#include "app/application/system/serializer.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/once.hpp"
#include "app/core/player/ping_pong.hpp"
#include "app/core/player/revert_loop.hpp"
#include "app/core/player/revert_once.hpp"
#include "app/core/player/stop.hpp"
#include "app/internal/serialization/all_serializers.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & p_version )
	{
		return { { "MAJOR", p_version.major }, { "MINOR", p_version.minor }, { "REVISION", p_version.revision } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version )
	{
		p_version.major	   = SERIALIZER().deserializeField<int>( p_json, "MAJOR" );
		p_version.minor	   = SERIALIZER().deserializeField<int>( p_json, "MINOR" );
		p_version.revision = SERIALIZER().deserializeField<int>( p_json, "REVISION" );
	}

	// VTX App
	Util::JSon::Object serialize( const VTXApp & p_app ) { throw NotImplementedException(); }
	void deserialize( const Util::JSon::Object & p_json, VTXApp & p_app ) { throw NotImplementedException(); }

	// Settings
	Util::JSon::Object serialize( const App::Application::Settings & p_settings )
	{
		const Application::Settings::SettingMap & settingMap = p_settings.getSettingMap();

		Util::JSon::Object settingMapJSon = Util::JSon::Object();

		for ( const auto & pair : settingMap )
		{
			settingMapJSon.appendField( pair.first, pair.second->serialize() );
		}

		return { { "MAP", settingMapJSon } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Application::Settings & p_settings )
	{
		const Application::Settings::SettingMap & settingMap = p_settings.getSettingMap();

		if ( !p_json.contains( "MAP" ) )
			throw( IOException( "Unreadable Setting file." ) );

		const Util::JSon::Object & settingMapJSon = p_json[ "MAP" ];

		for ( const Util::JSon::Object::Field & field : settingMapJSon )
		{
			if ( settingMap.contains( field.first ) )
			{
				try
				{
					settingMap.at( field.first )->deserialize( field.second );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "{}", e.what() );
					VTX_WARNING( "Unable to deserialize setting {}. Keep previous value.", field.first );
				}
			}
			else
			{
				VTX_WARNING( "Unknown setting \"{}\". Setting skipped.", field.first );
			}
		}
	}

	// TrajectoryPlayers
	Util::JSon::Object serialize( const App::Core::Player::BasePlayer & p_player )
	{
		return { { "COUNT", p_player.getCount() },
				 { "CURRENT", p_player.getCurrent() },
				 { "FPS", p_player.getFPS() },
				 { "IS_PLAYING", p_player.isPlaying() } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::BasePlayer & p_player )
	{
		p_player.setCount( SERIALIZER().deserializeField<size_t>( p_json, "COUNT" ) );
		p_player.setCurrent( SERIALIZER().deserializeField<size_t>( p_json, "CURRENT" ) );
		p_player.setFPS( SERIALIZER().deserializeField<uint>( p_json, "FPS" ) );

		if ( SERIALIZER().deserializeField<bool>( p_json, "IS_PLAYING", false ) )
			p_player.play();
	}
	Util::JSon::Object serialize( const App::Core::Player::Stop & p_player )
	{
		return SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Stop & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::Loop & p_player )
	{
		return SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Loop & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::RevertLoop & p_player )
	{
		return SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::RevertLoop & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::Once & p_player )
	{
		return SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Once & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::RevertOnce & p_player )
	{
		return SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::RevertOnce & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::PingPong & p_player )
	{
		Util::JSon::Object res
			= SERIALIZER().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );

		res.appendField( "IS_PLAYING_FORWARD", p_player.isPlayingForward() );
		return res;
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::PingPong & p_player )
	{
		SERIALIZER().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
		p_player.setPlayingForward( SERIALIZER().deserializeField<bool>( p_json, "IS_PLAYING_FORWARD" ) );
	}
} // namespace VTX::App::Internal::Serialization
