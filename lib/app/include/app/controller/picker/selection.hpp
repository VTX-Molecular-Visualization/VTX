#ifndef __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__
#define __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__

#include "app/core/controller/base_picker_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include <app/application/selection/picking_info.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Picker
{

	class Selection : public Core::Controller::BasePickerController
	{
	  public:
		using PickingInfo = App::Application::Selection::PickingInfo;

		inline static const Util::CollectionKey COLLECTION_ID		 = "CONTROLLER_PICKER";
		inline static const VTX::Hash			HASHED_COLLECTION_ID = Util::hash( COLLECTION_ID );

	  private:
		inline static const Core::Controller::ControllerCollection::Registration<Selection> _reg { COLLECTION_ID };

	  public:
		Selection()								= default;
		Selection( const Selection & p_source ) = default;
		~Selection()							= default;

		void init() override;

		inline VTX::Hash				getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Selection>( *this ); };

	  protected:
		void _onMouseLeftClick( const Vec2i & p_mousePos );
		void _onMouseLeftDoubleClick( const Vec2i & p_mousePos );
		void _onMouseRightClick( const Vec2i & p_mousePos );

		void _performSelection( const PickingInfo & p_pickingInfo ) const;
		bool _isTargetSelected( const PickingInfo & p_pickingInfo ) const;

	  private:
		PickingInfo _lastPickingInfo = PickingInfo();
	};
} // namespace VTX::App::Controller::Picker

#endif
