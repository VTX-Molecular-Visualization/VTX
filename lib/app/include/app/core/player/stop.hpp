#ifndef __VTX_APP_CORE_PLAYER_STOP__
#define __VTX_APP_CORE_PLAYER_STOP__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
{
	class Stop : public BasePlayer
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID  = "STOP";
		inline static const std::string			DISPLAYED_NAME = "Stop";

	  private:
		inline static const Players::Registration<Stop> _reg { COLLECTION_ID };

	  public:
		Stop();
		Stop( const Stop & p_source );

		void play() override;

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const Util::CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<Stop>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::Player
#endif
