#ifndef __VTX_COMPONENT_CONSOLE__
#define __VTX_COMPONENT_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "localization/language.hpp"
#include "util/logger.hpp"
#include <list>

namespace VTX
{
	namespace UI
	{
		class Console : public BaseComponent
		{
		  public:
			virtual std::string getName() const override { return ID::UI::CONSOLE; }
			virtual void		receiveEvent( const Event::VTX_EVENT & p_event, void * const p_arg = nullptr ) override;

		  protected:
			virtual void						  _draw() override;
			virtual std::vector<Event::VTX_EVENT> _getEvents() const override
			{
				return std::vector<Event::VTX_EVENT>( { Event::Global::LOG_CONSOLE } );
			}

		  private:
			std::list<Util::Logger::Log> _logs = std::list<Util::Logger::Log>();

			void _addLog( const Util::Logger::Log & );
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
