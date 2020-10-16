#ifndef __VTX_BASE_VIEW__
#define __VTX_BASE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "model/base_model.hpp"

namespace VTX
{
	namespace View
	{
		class BaseView
		{
		  public:
			explicit BaseView( VTX::Model::BaseModel * const p_model ) {};
			virtual ~BaseView() = default;

			virtual void notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				if ( p_event == Event::VTX_EVENT_MODEL::DATA_CHANGE )
					refreshView();
			};

			inline virtual void refreshView() {};
		};
	} // namespace View
} // namespace VTX
#endif
