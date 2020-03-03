#ifndef __VTX_ACTION_UNSELECT__
#define __VTX_ACTION_UNSELECT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Unselect : public BaseAction
		{
		  public:
			Unselect( Generic::BaseSelectable & p_selectable ) : _selectable( p_selectable ) {};
			virtual void execute() override { _selectable.setSelected( true ); };

		  private:
			Generic::BaseSelectable & _selectable;
		};
	} // namespace Action
} // namespace VTX
#endif
