#ifndef __VTX_ACTION_SELECT__
#define __VTX_ACTION_SELECT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "selection/base_selectable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Select : public BaseAction
		{
		  public:
			Select( Generic::BaseSelectable & p_selectable ) : _selectable( p_selectable ) {};
			virtual void execute() override { VTXApp::get().getSelectionManager().select( &_selectable ); };

		  private:
			Generic::BaseSelectable & _selectable;
		};
	} // namespace Action
} // namespace VTX
#endif
