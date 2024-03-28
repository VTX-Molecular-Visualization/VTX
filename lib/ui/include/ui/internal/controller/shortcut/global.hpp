#ifndef __VTX_UI_INTERNAL_CONTROLLER_GLOBAL_SHORTCUT__
#define __VTX_UI_INTERNAL_CONTROLLER_GLOBAL_SHORTCUT__

#include "ui/core/input/keys.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/base_shortcut_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <memory>
#include <util/hashing.hpp>

namespace VTX::UI::Internal::Controller::Shortcut
{
	using namespace VTX::UI::QT;
	using namespace VTX::UI::QT::Controller;

	class Global final : public BaseShortcutController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_SHORTCUT_GLOBAL";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<Global> _reg { COLLECTION_ID };

	  public:
		Global()						  = default;
		Global( const Global & p_source ) = default;
		~Global()						  = default;

		void init() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Global>( *this ); };
	};
} // namespace VTX::UI::Internal::Controller::Shortcut
#endif
