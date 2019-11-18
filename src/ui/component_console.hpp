#ifndef __VTX_COMPONENT_CONSOLE__
#define __VTX_COMPONENT_CONSOLE__

#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../util/logger.hpp"
#include "base_component.hpp"
#include <vector>

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
			virtual void _applyEvent( const Event::EVENT_UI, void * p_arg ) override;

		  private:
			std::vector<Util::Logger::Log> _logs = std::vector<Util::Logger::Log>();

			void _addLog( const Util::Logger::Log & );
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
