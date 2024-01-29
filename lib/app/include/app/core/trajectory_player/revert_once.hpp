#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_ONCE__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_ONCE__

#include "base_player.hpp"
#include "players.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class RevertOnce : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "REVERT_ONCE";
		inline static const std::string	  DISPLAYED_NAME = "Revert Once";

	  private:
		inline static const Players::Registration<RevertOnce> _reg { COLLECTION_ID };

	  public:
		RevertOnce()							  = default;
		RevertOnce( const RevertOnce & p_source ) = default;
		RevertOnce( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string &	  getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
