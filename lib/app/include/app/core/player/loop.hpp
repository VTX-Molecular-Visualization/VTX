#ifndef __VTX_APP_CORE_PLAYER_LOOP__
#define __VTX_APP_CORE_PLAYER_LOOP__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
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

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const CollectionKey &		getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<Loop>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::Player
#endif
