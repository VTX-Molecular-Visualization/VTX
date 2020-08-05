#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

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
		class BaseModel : public Generic::HasCollection<View::BaseView<BaseModel>>, public Selection::BaseSelectable
		{
		  public:
			inline static long COUNTER = 0;

			long getId() const { return _id; }

		  protected:
			void _notifyViews( const Event::VTX_EVENT_MODEL p_event )
			{
				for ( PairStringToItemPtr & pair : _getItems() )
				{
					pair.second->notify( p_event );
				}
			}

			long _id = COUNTER++;
		};
	} // namespace Model
} // namespace VTX
#endif
