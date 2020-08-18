#ifndef __VTX_ACTION_SELECTABLE__
#define __VTX_ACTION_SELECTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "selection/selection_manager.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Selectable
		{
			template<typename T, typename = std::enable_if<std::is_base_of<Selection::BaseSelectable, T>::value>>
			class SetSelected : public BaseAction
			{
			  public:
				explicit SetSelected( T & p_selectable, const bool p_selected ) :
					_selectable( p_selectable ), _selected( p_selected ) {};

				virtual void execute() override
				{
					_selectable.setSelected( _selected );
					if ( _selected )
					{
						VTXApp::get().getSelectionManager().select( &_selectable );
					}
					else
					{
						VTXApp::get().getSelectionManager().unselect( &_selectable );
					}
				};

			  protected:
				T &	 _selectable;
				bool _selected;
			};
		} // namespace Selectable
	}	  // namespace Action
} // namespace VTX
#endif
