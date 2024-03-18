#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_STOP__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_STOP__

#include "base_player.hpp"
#include "players.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	class Stop : public BasePlayer
	{
	  public:
		inline static const CollectionKey COLLECTION_ID	 = "STOP";
		inline static const std::string	  DISPLAYED_NAME = "Stop";

	  private:
		inline static const Players::Registration<Stop> _reg { COLLECTION_ID };

	  public:
		Stop();
		Stop( const Stop & p_source );

		void play() override;

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey &		getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<Stop>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
