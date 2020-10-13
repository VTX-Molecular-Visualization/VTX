#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "selection/base_selectable.hpp"
#include <string>

namespace VTX
{
	namespace Model
	{
		using Model_ID = unsigned long;

		class BaseModel : public Selection::BaseSelectable
		{
		  public:
			inline static Model_ID COUNTER = 0;

			const Model_ID & getId() const { return _id; };
			void			 instantiateDefaultViews() {};

			bool isEnable() { return _enabled; };
			void setEnable( bool p_enable ) { _enabled = p_enable; };

		  protected:
			Model_ID _id	  = COUNTER++;
			bool	 _enabled = true;

			void _notifyViews( const Event::VTX_EVENT_MODEL & p_event );
		};
	} // namespace Model
} // namespace VTX
#endif
