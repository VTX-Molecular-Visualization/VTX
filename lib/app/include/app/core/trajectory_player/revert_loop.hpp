#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_LOOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_REVERT_LOOP__

#include "base_player.hpp"
#include "players.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class RevertLoop : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "REVERT_LOOP";
		inline static const std::string	  DISPLAYED_NAME = "Revert Loop";

	  private:
		inline static const Players::Registration<RevertLoop> _reg { COLLECTION_ID };

	  public:
		RevertLoop()							  = default;
		RevertLoop( const RevertLoop & p_source ) = default;
		RevertLoop( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey &		getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<RevertLoop>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
