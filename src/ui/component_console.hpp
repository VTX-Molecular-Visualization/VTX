#ifndef __VTX_COMPONENT_CONSOLE__
#define __VTX_COMPONENT_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../localization/language.hpp"
#include "../util/logger.hpp"
#include "base_component.hpp"
#include <list>

namespace VTX
{
	namespace UI
	{
		class ComponentConsole : public BaseComponent
		{
		  public:
			ComponentConsole( bool * const );

		  protected:
			virtual void _draw() override;
			virtual void _registerEventHandlers() override;
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) override;

			virtual COMPONENT_NAME getName() const override { return COMPONENT_NAME::CONSOLE; }

		  private:
			std::list<Util::Logger::Log> _logs = std::list<Util::Logger::Log>();

			void _addLog( const Util::Logger::Log & );
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
