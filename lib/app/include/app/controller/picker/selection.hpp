#ifndef __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__
#define __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__

#include "app/core/controller/concepts.hpp"
#include <app/application/selection/picking_info.hpp>
#include <util/callback.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Picker
{

	class Selection : public Core::Controller::BaseController
	{
	  public:
		using PickingInfo = App::Application::Selection::PickingInfo;

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
