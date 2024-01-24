#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_ONCE__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_ONCE__

#include "base_player.hpp"
#include "players.hpp"
#include <string>

namespace VTX::App::Core::TrajectoryPlayer
{
	class Once : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "ONCE";
		inline static const std::string	  DISPLAYED_NAME = "Once";

	  private:
		inline static const Players::Registration<Once> _reg { COLLECTION_ID };

	  public:
		Once()						  = default;
		Once( const Once & p_source ) = default;
		Once( VTX::Core::Struct::Trajectory * const p_trajectory );

		const std::string &	  getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		std::unique_ptr<BasePlayer> clone() const override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
