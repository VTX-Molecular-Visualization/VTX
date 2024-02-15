#ifndef __VTX_UI_QT_CONTROLLER_SELECTION_PICKER__
#define __VTX_UI_QT_CONTROLLER_SELECTION_PICKER__

#include "ui/qt/controller/base_picker_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <app/application/selection/picking_info.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class SelectionPicker : public BasePickerController
	{
	  public:
		using PickingInfo = App::Application::Selection::PickingInfo;

		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_PICKER";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<SelectionPicker> _reg { COLLECTION_ID };

	  public:
		SelectionPicker()									= default;
		SelectionPicker( const SelectionPicker & p_source ) = default;
		~SelectionPicker()									= default;

		void init() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<SelectionPicker>( *this ); };

	  protected:
		void _onMouseLeftClick( const Vec2i & p_mousePos );
		void _onMouseLeftDoubleClick( const Vec2i & p_mousePos );
		void _onMouseRightClick( const Vec2i & p_mousePos );

		void _performSelection( const PickingInfo & p_pickingInfo ) const;
		bool _isTargetSelected( const PickingInfo & p_pickingInfo ) const;

	  private:
		PickingInfo _lastPickingInfo = PickingInfo();
	};
} // namespace VTX::UI::QT::Controller

#endif
