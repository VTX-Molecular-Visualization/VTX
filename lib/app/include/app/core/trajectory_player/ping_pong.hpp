#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_PING_PONG__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_PING_PONG__

#include "base_player.hpp"
#include "players.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class PingPong : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "PING_PONG";
		inline static const std::string	  DISPLAYED_NAME = "Ping Pong";

	  private:
		inline static const Players::Registration<PingPong> _reg { COLLECTION_ID };

	  public:
		PingPong()							  = default;
		PingPong( const PingPong & p_source ) = default;
		PingPong( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string &	  getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		bool isPlayingForward() const { return _forward; }
		void setPlayingForward( const bool p_forward ) { _forward = p_forward; }

		std::unique_ptr<BasePlayer> clone() const override;

	  private:
		bool _forward = true;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
