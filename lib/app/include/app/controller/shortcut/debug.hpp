#ifndef __VTX_UI_INTERNAL_CONTROLLER_DEBUG_SHORTCUT__
#define __VTX_UI_INTERNAL_CONTROLLER_DEBUG_SHORTCUT__

#include "app/core/controller/base_shortcut_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include <memory>
#include <util/hashing.hpp>

namespace VTX::App::Controller::Shortcut
{
	class Debug final : public Core::Controller::BaseShortcutController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_SHORTCUT_DEBUG";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const Core::Controller::ControllerCollection::Registration<Debug> _reg { COLLECTION_ID };

	  public:
		Debug()							= default;
		Debug( const Debug & p_source ) = default;
		~Debug()						= default;

		void init() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Debug>( *this ); };
	};
} // namespace VTX::App::Controller::Shortcut
#endif
