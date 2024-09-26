#include "app/serialization/app_serializers.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/once.hpp"
#include "app/core/player/ping_pong.hpp"
#include "app/core/player/revert_loop.hpp"
#include "app/core/player/revert_once.hpp"
#include "app/core/player/stop.hpp"
#include "app/serialization/serialization_system.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & p_version )
	{
		return { { "MAJOR", p_version.major }, { "MINOR", p_version.minor }, { "REVISION", p_version.revision } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version )
	{
		p_version.major	   = SERIALIZATION_SYSTEM().deserializeField<int>( p_json, "MAJOR" );
		p_version.minor	   = SERIALIZATION_SYSTEM().deserializeField<int>( p_json, "MINOR" );
		p_version.revision = SERIALIZATION_SYSTEM().deserializeField<int>( p_json, "REVISION" );
	}

	// VTX App
	Util::JSon::Object serialize( const VTXApp & p_app ) { throw NotImplementedException(); }
	void deserialize( const Util::JSon::Object & p_json, VTXApp & p_app ) { throw NotImplementedException(); }

	// Settings
	template<typename T>
	void deserializeSetting( const Util::JSon::Object & p_json, T & p_setting )
	{
		p_setting.deserialize( p_json );
	}

	Util::JSon::Object serialize( const App::Core::Settings::SettingMap & p_settings )
	{
		Util::JSon::Object settingMapJSon = Util::JSon::Object();

		for ( const auto & pair : p_settings )
		{
			settingMapJSon.appendField( pair.first, pair.second->serialize() );
		}

		return { { "MAP", settingMapJSon } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Settings::SettingMap & p_settings )
	{
		if ( !p_json.contains( "MAP" ) )
			throw( IOException( "Unreadable Setting file." ) );

		const Util::JSon::Object & settingMapJSon = p_json[ "MAP" ];

		for ( const Util::JSon::Object::Field & field : settingMapJSon )
		{
			if ( p_settings.contains( field.first ) )
			{
				try
				{
					p_settings.at( field.first )->deserialize( field.second );
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
		p_player.setCount( SERIALIZATION_SYSTEM().deserializeField<size_t>( p_json, "COUNT" ) );
		p_player.setCurrent( SERIALIZATION_SYSTEM().deserializeField<size_t>( p_json, "CURRENT" ) );
		p_player.setFPS( SERIALIZATION_SYSTEM().deserializeField<uint>( p_json, "FPS" ) );

		if ( SERIALIZATION_SYSTEM().deserializeField<bool>( p_json, "IS_PLAYING", false ) )
			p_player.play();
	}
	Util::JSon::Object serialize( const App::Core::Player::Stop & p_player )
	{
		return SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Stop & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::Loop & p_player )
	{
		return SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Loop & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::RevertLoop & p_player )
	{
		return SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::RevertLoop & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::Once & p_player )
	{
		return SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::Once & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::RevertOnce & p_player )
	{
		return SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::RevertOnce & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
	}
	Util::JSon::Object serialize( const App::Core::Player::PingPong & p_player )
	{
		Util::JSon::Object res
			= SERIALIZATION_SYSTEM().serialize( static_cast<const App::Core::Player::BasePlayer &>( p_player ) );

		res.appendField( "IS_PLAYING_FORWARD", p_player.isPlayingForward() );
		return res;
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Player::PingPong & p_player )
	{
		SERIALIZATION_SYSTEM().deserialize( p_json, static_cast<const App::Core::Player::BasePlayer &>( p_player ) );
		p_player.setPlayingForward( SERIALIZATION_SYSTEM().deserializeField<bool>( p_json, "IS_PLAYING_FORWARD" ) );
	}
} // namespace VTX::App::Serialization
