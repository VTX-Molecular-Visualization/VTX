#ifndef __VTX_UI_QT_CONTROLLER_VISUALIZATION_SHORTCUT__
#define __VTX_UI_QT_CONTROLLER_VISUALIZATION_SHORTCUT__

#include "ui/core/input/keys.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/base_shortcut_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <memory>
#include <util/hashing.hpp>

namespace VTX::UI::QT::Controller
{
	class VisualizationShortcut final : public BaseShortcutController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_SHORTCUT_VISUALIZATION";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<VisualizationShortcut> _reg { COLLECTION_ID };

	  public:
		VisualizationShortcut()											= default;
		VisualizationShortcut( const VisualizationShortcut & p_source ) = default;
		~VisualizationShortcut()										= default;

		void init() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<VisualizationShortcut>( *this ); };

	  private:
		void _callOrientOnScene() const;
		void _callOrientOnSelection() const;
	};
} // namespace VTX::UI::QT::Controller
#endif
