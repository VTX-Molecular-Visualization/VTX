#ifndef __COMPONENT_CONSOLE__
#define __COMPONENT_CONSOLE__

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
			ComponentConsole( bool * );
			void addLog( const Util::Logger::Log );

		  protected:
			virtual void _draw() override;

		  private:
			std::vector<Util::Logger::Log> _logs
				= std::vector<Util::Logger::Log>();

			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
