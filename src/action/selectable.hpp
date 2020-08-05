#ifndef __VTX_ACTION_SELECTABLE__
#define __VTX_ACTION_SELECTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Selectable
		{
			class SetSelected : public BaseAction
			{
			  public:
				explicit SetSelected( Selection::BaseSelectable & p_selectable, const bool p_selected ) :
					_selectable( &p_selectable ), _selected( p_selected ) {};

				virtual void execute() override
				{
					if ( _selected )
					{
						VTXApp::get().getSelectionManager().select( _selectable );
					}
					else
					{
						VTXApp::get().getSelectionManager().unselect( _selectable );
					}
				};

			  protected:
				Selection::BaseSelectable * const _selectable;
				bool							  _selected;
			};

			class SetPathSelected : public SetSelected
			{
			  public:
				explicit SetPathSelected( Model::Path & p_path, const bool p_selected ) :
					SetSelected( p_path, p_selected )
				{
				}

				virtual void execute() override
				{
					SetSelected::execute();
					( (Model::Path * const)_selected )->setSelected();
				}
			};
		} // namespace Selectable
	}	  // namespace Action
} // namespace VTX
#endif
