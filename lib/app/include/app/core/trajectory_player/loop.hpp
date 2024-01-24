#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_LOOP__

#include "app/core/collection.hpp"
#include "base_player.hpp"
#include "ping_pong.hpp"
#include "players.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	class Loop : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "LOOP";
		inline static const std::string	  DISPLAYED_NAME = "Loop";

	  private:
		inline static const Players::Registration<Loop> _reg { COLLECTION_ID };

	  public:
		Loop()						  = default;
		Loop( const Loop & p_source ) = default;
		Loop( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string &	  getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		std::unique_ptr<BasePlayer> clone() const override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
