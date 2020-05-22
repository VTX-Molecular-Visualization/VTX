#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "generic/base_drawable.hpp"
#include "generic/has_collection.hpp"
#include "id.hpp"
#include "imgui/imgui.h"
#include "localization/language.hpp"
#include "model/base_model.hpp"
#include <map>
#include <set>
#include <vector>

namespace VTX
{
	namespace UI
	{
		class BaseComponent :
			public Generic::HasCollection<Generic::BaseDrawable>,
			public Generic::BaseDrawable,
			public Event::BaseEventReceiverVTX
		{
		  public:
			virtual ~BaseComponent() {}

			virtual void		  init() override;
			virtual void		  clean() override;
			BaseComponent * const getComponentByName( const std::string & );

		  protected:
			virtual bool _drawHeader() = 0;
			virtual void _drawContent();
			virtual void _drawFooter() = 0;
			virtual void _draw() override;
			virtual void _drawComponent( const std::string & );
			virtual void _drawComponents() final;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
