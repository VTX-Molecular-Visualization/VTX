#ifndef __VTX_ACTION_VISIBLE_CHANGE_VISIBILITY__
#define __VTX_ACTION_VISIBLE_CHANGE_VISIBILITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_visible.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class VisibleChangeVisibility : public BaseAction
		{
		  public:
			explicit VisibleChangeVisibility( Generic::BaseVisible & p_visible, const bool p_visibility ) :
				_visible( p_visible ), _visibility( p_visibility )
			{
			}

			virtual void execute() override { _visible.setVisible( _visibility ); }

		  private:
			Generic::BaseVisible & _visible;
			const bool			   _visibility;
		};
	} // namespace Action
} // namespace VTX
#endif
