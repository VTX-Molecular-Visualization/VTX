#ifndef __VTX_ACTION_SELECT__
#define __VTX_ACTION_SELECT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Select : public BaseAction
		{
		  public:
			Select( Selection::BaseSelectable & p_selectable ) : _selectable( p_selectable ) {};
			virtual void execute() override { VTXApp::get().getSelectionManager().select( &_selectable ); };

		  private:
			Selection::BaseSelectable & _selectable;
		};
	} // namespace Action
} // namespace VTX
#endif
