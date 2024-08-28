#ifndef __VTX_UI_INTERNAL_CONTROLLER_GLOBAL_SHORTCUT__
#define __VTX_UI_INTERNAL_CONTROLLER_GLOBAL_SHORTCUT__

#include "app/core/controller/base_controller.hpp"
#include "app/core/controller/base_shortcut_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include <memory>
#include <util/hashing.hpp>

namespace VTX::App::Controller::Shortcut
{

	class Global final : public Core::Controller::BaseShortcutController
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID		 = "CONTROLLER_SHORTCUT_GLOBAL";
		inline static const VTX::Hash			HASHED_COLLECTION_ID = Util::hash( COLLECTION_ID );

	  private:
		inline static const Core::Controller::ControllerCollection::Registration<Global> _reg { COLLECTION_ID };

	  public:
		Global()						  = default;
		Global( const Global & p_source ) = default;
		~Global()						  = default;

		void init() override;

		inline VTX::Hash				getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Global>( *this ); };
	};
} // namespace VTX::App::Controller::Shortcut
#endif
