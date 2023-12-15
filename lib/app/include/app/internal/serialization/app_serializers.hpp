#ifndef __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__
#define __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__

#include "app/application/_fwd.hpp"
#include "app/core/serialization/version.hpp"
#include "app/core/trajectory_player/_fwd.hpp"
#include <util/json/json.hpp>

namespace VTX::App
{
	class VTXApp;
}

namespace VTX::App::Internal::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Serialization::Version & );

	// VTXApp
	Util::JSon::Object serialize( const VTXApp & );
	void			   deserialize( const Util::JSon::Object &, VTXApp & );

	// Settings
	Util::JSon::Object serialize( const App::Application::Settings & );
	void			   deserialize( const Util::JSon::Object &, App::Application::Settings & );

	// TrajectoryPlayers
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::BasePlayer & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::BasePlayer & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::Stop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::Stop & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::Loop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::Loop & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::RevertLoop & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::RevertLoop & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::Once & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::Once & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::RevertOnce & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::RevertOnce & );
	Util::JSon::Object serialize( const App::Core::TrajectoryPlayer::PingPong & );
	void			   deserialize( const Util::JSon::Object &, App::Core::TrajectoryPlayer::PingPong & );

} // namespace VTX::App::Internal::Serialization
#endif
