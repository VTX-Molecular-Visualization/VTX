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
			explicit Console( bool * const );

			virtual std::string getName() const override { return ID::UI::CONSOLE; }

		  protected:
			virtual void _addItems() override {}
			virtual void _draw() override;
			virtual void _registerEventHandlers() override;
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) override;

		  private:
			std::list<Util::Logger::Log> _logs = std::list<Util::Logger::Log>();

			void _addLog( const Util::Logger::Log & );
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
