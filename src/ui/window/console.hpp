#ifndef __VTX_COMPONENT_WINDOW__CONSOLE__
#define __VTX_COMPONENT_WINDOW__CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_window.hpp"
#include <list>

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			class Console : public BaseComponentWindow
			{
			  public:
				Console();

				virtual const std::string & getName() const override { return ID::UI::CONSOLE; }
				virtual const char *		getTitle() const override { return LOCALE( "Console.Console" ); }
				virtual void				receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual void _drawContent() override;

			  private:
				std::list<Event::VTXEventLog> _logs = std::list<Event::VTXEventLog>();

				void _addLog( const Event::VTXEventLog & );
				void _clear();
			};
		} // namespace Window
	}	  // namespace UI
} // namespace VTX
#endif
