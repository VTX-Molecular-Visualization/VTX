#ifndef __VTX_COMPONENT_CONSOLE__
#define __VTX_COMPONENT_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_window.hpp"
#include "util/logger.hpp"
#include <list>

namespace VTX
{
	namespace UI
	{
		class Console : public BaseComponentWindow
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::CONSOLE; }
			virtual const char *		getTitle() const override { return LOCALE( "Console.Console" ); }
			virtual void				receiveEvent( const Event::VTXEvent & p_event ) override;

		  protected:
			virtual void						  _drawContent() override;
			virtual std::vector<Event::VTX_EVENT> _getEvents() const override
			{
				return std::vector<Event::VTX_EVENT>( { Event::Global::LOG_CONSOLE } );
			}

		  private:
			std::list<Event::VTXEventLog> _logs = std::list<Event::VTXEventLog>();

			void _addLog( const Event::VTXEventLog & );
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
