#ifndef __VTX_COMPONENT_PROGRESS_BAR__
#define __VTX_COMPONENT_PROGRESS_BAR__

#ifdef _MSC_VER
#pragma once
#endif

#include "localization/language.hpp"
#include "ui/base_component.hpp"
#include "util/logger.hpp"
#include <list>

namespace VTX
{
	namespace UI
	{
		namespace Modal
		{
			class ProgressBar : public BaseComponent
			{
			  public:
				virtual const std::string & getName() const override { return ID::UI::PROGRESS_BAR; }
				virtual void		receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual void						  _draw() override;
				virtual std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::UPDATE_PROGRESS_BAR } );
				}

			  private:
				float _value = 0.f;
			};
		} // namespace Modal
	}	  // namespace UI
} // namespace VTX
#endif
