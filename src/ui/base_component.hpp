#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "../event/event.hpp"
#include "../generic/base_drawable.hpp"
#include "../generic/has_collection.hpp"
#include "../id.hpp"
#include "../model/base_model.hpp"
#include "imgui/imgui.h"
#include <map>
#include <set>

namespace VTX
{
	namespace UI
	{
		class BaseComponent : public Generic::HasCollection<Generic::BaseDrawable>, public Generic::BaseDrawable
		{
		  public:
			BaseComponent( bool * const p_show ) { _show = p_show; }

			virtual void display();
			virtual void init() override;

			virtual const UI::BaseComponent * getComponentByName( const std::string & ) const final;
			virtual void					  receiveEvent( const Event::EVENT_UI, void * const ) final;

		  protected:
			virtual void _drawComponent( const std::string & );
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) {};

		  private:
			std::set<Event::EVENT_UI> _events = std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
