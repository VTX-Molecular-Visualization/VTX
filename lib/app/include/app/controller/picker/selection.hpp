#ifndef __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__
#define __VTX_UI_INTERNAL_CONTROLLER_PICKER_SELECTION_PICKER__

#include "app/core/controller/concepts.hpp"
#include <app/application/selection/picking_info.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Picker
{

	class Selection : public Core::Controller::BaseController
	{
	  public:
		using PickingInfo = App::Application::Selection::PickingInfo;

	  public:
		void init();
		void update( const float p_deltaTime );

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
