#ifndef __VTX_BASE__
#define __VTX_BASE__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/has_collection.hpp"
#include "math/transform.hpp"
#include "selection/base_selectable.hpp"
#include "view/base_view.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class BaseModel : public Generic::HasCollection<View::BaseView<BaseModel>>, public Generic::BaseSelectable
		{
		  public:
			uint getId() const { return _id; }
			void setId( const uint p_id ) { _id = p_id; }

		  protected:
			virtual void _notifyViews( const Event::VTX_EVENT_MODEL p_event ) final
			{
				for ( PairStringToItemPtr pair : _getItems() )
				{
					pair.second->notify( p_event );
				}
			}

			uint _id = INVALID_ID;
		};
	} // namespace Model
} // namespace VTX
#endif
