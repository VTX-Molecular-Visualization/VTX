#ifndef __VTX_APP_SERIALIZATION_APP_SERIALIZERS__
#define __VTX_APP_SERIALIZATION_APP_SERIALIZERS__

#include "app/application/_fwd.hpp"
#include "app/application/settings/_fwd.hpp"
#include "app/core/player/_fwd.hpp"
#include "app/core/serialization/version.hpp"
#include <util/json/json.hpp>

namespace VTX::App
{
	class VTXApp;
}

namespace VTX::App::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Serialization::Version & );

	// VTXApp
	Util::JSon::Object serialize( const VTXApp & );
	void			   deserialize( const Util::JSon::Object &, VTXApp & );

	// Settings
	Util::JSon::Object serialize( const App::Application::Settings::Settings & );
	void			   deserialize( const Util::JSon::Object &, App::Application::Settings::Settings & );

	// TrajectoryPlayers
	Util::JSon::Object serialize( const App::Core::Player::BasePlayer & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::BasePlayer & );
	Util::JSon::Object serialize( const App::Core::Player::Stop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::Stop & );
	Util::JSon::Object serialize( const App::Core::Player::Loop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::Loop & );
	Util::JSon::Object serialize( const App::Core::Player::RevertLoop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::RevertLoop & );
	Util::JSon::Object serialize( const App::Core::Player::Once & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::Once & );
	Util::JSon::Object serialize( const App::Core::Player::RevertOnce & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::RevertOnce & );
	Util::JSon::Object serialize( const App::Core::Player::PingPong & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Player::PingPong & );

} // namespace VTX::App::Serialization
#endif
