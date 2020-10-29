#ifndef __VTX_ACTION_VISIBLE__
#define __VTX_ACTION_VISIBLE__

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
		namespace Visible
		{
			class ChangeVisibility : public BaseAction
			{
			  public:
				enum class VISIBILITY_MODE : int
				{
					HIDE = 0,
					SHOW,
					TOGGLE,
					SOLO,
					ALL
				};

				explicit ChangeVisibility( Generic::BaseVisible & p_visible, const VISIBILITY_MODE p_mode ) : _visible( p_visible ), _mode( p_mode ) {}

				virtual void execute() override
				{
					const bool newVisibility = _getVisibilityBool();
					_visible.setVisible( newVisibility );
				}

			  protected:
				Generic::BaseVisible & _visible;
				const VISIBILITY_MODE  _mode;

				inline bool _getVisibilityBool()
				{
					bool newVisibility;
					switch ( _mode )
					{
					case VISIBILITY_MODE::SHOW:
					case VISIBILITY_MODE::SOLO:
					case VISIBILITY_MODE::ALL: newVisibility = true; break;
					case VISIBILITY_MODE::HIDE: newVisibility = false; break;
					case VISIBILITY_MODE::TOGGLE: newVisibility = !_visible.isVisible(); break;
					default: newVisibility = _visible.isVisible(); break;
					}

					return newVisibility;
				}
			};
		} // namespace Visible
	}	  // namespace Action
} // namespace VTX
#endif
