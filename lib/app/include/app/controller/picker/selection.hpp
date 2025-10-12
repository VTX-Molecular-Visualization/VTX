#ifndef __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__
#define __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__

#include "app/controller/base_controller.hpp"
#include <app/selection/picking_info.hpp>
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Picker
{

	class Selection : public BaseController
	{
	  public:
		using PickingInfo = App::Selection::PickingInfo;

		Selection() { setActive( true ); }
		~Selection()
		{
			// TODO: fixme
			// setActive( false );
		}

		void setActive( const bool );
		void update( const float, const float ) {}

	  private:
		Util::CallbackId _mouseLeftClickCallbackID;
		Util::CallbackId _mouseLeftDoubleClickCallbackID;
		Util::CallbackId _mouseRightClickCallbackID;

		void _onMouseLeftClick( const Vec2i & p_mousePos );
		void _onMouseLeftDoubleClick( const Vec2i & p_mousePos );
		void _onMouseRightClick( const Vec2i & p_mousePos );

		void		_performSelection( const PickingInfo & p_pickingInfo ) const;
		bool		_isTargetSelected( const PickingInfo & p_pickingInfo ) const;
		PickingInfo _lastPickingInfo = PickingInfo();
	};
} // namespace VTX::App::Controller::Picker

#endif
