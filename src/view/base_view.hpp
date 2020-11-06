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
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView
		{
		  public:
			explicit BaseView( T * const p_model ) : _model( p_model ) {}
			virtual ~BaseView() = default;

			virtual void notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData = 0 )
			{
				if ( p_event == Event::VTX_EVENT_MODEL::DATA_CHANGE )
				{
					refreshView();
				}
			};

			inline virtual void refreshView() {};

		  protected:
			T * const _model;
		};
	} // namespace View
} // namespace VTX
#endif
