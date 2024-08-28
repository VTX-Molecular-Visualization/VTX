#ifndef __VTX_APP_CORE_PLAYER_REVERT_ONCE__
#define __VTX_APP_CORE_PLAYER_REVERT_ONCE__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
{
	class RevertOnce : public BasePlayer
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID  = "REVERT_ONCE";
		inline static const std::string			DISPLAYED_NAME = "Revert Once";

	  private:
		inline static const Players::Registration<RevertOnce> _reg { COLLECTION_ID };

	  public:
		RevertOnce()							  = default;
		RevertOnce( const RevertOnce & p_source ) = default;

		const std::string &			getDisplayName() const override { return DISPLAYED_NAME; }
		const Util::CollectionKey & getCollectionKey() const override { return COLLECTION_ID; }
		std::unique_ptr<BasePlayer> clone() const override { return std::make_unique<RevertOnce>( *this ); }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
	};
} // namespace VTX::App::Core::Player
#endif
