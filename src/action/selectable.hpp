#ifndef __VTX_ACTION_SELECTABLE__
#define __VTX_ACTION_SELECTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Selectable
		{
			class Select : public BaseAction
			{
			  public:
				Select( Selection::BaseSelectable & p_selectable ) : _selectable( p_selectable ) {};
				virtual void execute() override { VTXApp::get().getSelectionManager().select( &_selectable ); };

			  private:
				Selection::BaseSelectable & _selectable;
			};

			class Unselect : public BaseAction
			{
			  public:
				Unselect( Selection::BaseSelectable & p_selectable ) : _selectable( p_selectable ) {};
				virtual void execute() override { VTXApp::get().getSelectionManager().unselect( &_selectable ); }

			  private:
				Selection::BaseSelectable & _selectable;
			}; // namespace VTX
		}	   // namespace Selectable
	}		   // namespace Action
} // namespace VTX
#endif
