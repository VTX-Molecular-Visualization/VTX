#ifndef __VTX_APP_CORE_PLAYER_REVERT_LOOP__
#define __VTX_APP_CORE_PLAYER_REVERT_LOOP__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
{
	class RevertLoop : public BasePlayer
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID  = "REVERT_LOOP";
		inline static const std::string			DISPLAYED_NAME = "Revert Loop";

	  private:
		inline static const Players::Registration<RevertLoop> _reg { COLLECTION_ID };

	  public:
		RevertLoop()							  = default;
		RevertLoop( const RevertLoop & p_source ) = default;

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const Util::CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<RevertLoop>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::Player
#endif
