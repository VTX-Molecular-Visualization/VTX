#ifndef __VTX_UI_QT_CONTROLLER_GLOBAL_SHORTCUT__
#define __VTX_UI_QT_CONTROLLER_GLOBAL_SHORTCUT__

#include "ui/internal/all_settings.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/controller/shortcut.hpp"
#include "ui/qt/controller/shortcut_map.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <util/hashing.hpp>

namespace VTX::UI::QT::Controller
{
	class GlobalShortcut : public BaseController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_SHORTCUT_GLOBAL";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<GlobalShortcut> _reg { COLLECTION_ID };

	  public:
		GlobalShortcut()								  = default;
		GlobalShortcut( const GlobalShortcut & p_source ) = default;
		~GlobalShortcut()								  = default;

		void init() override;
		void referenceShortcut( const Shortcut & p_shortcut );

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<GlobalShortcut>( *this ); };

	  protected:
		void _onKeyPressed( Input::Key p_key ) const;

	  private:
		ShortcutMap _mapShortcuts = ShortcutMap();
	};
} // namespace VTX::UI::QT::Controller
#endif
