#ifndef __COMPONENT_CONSOLE__
#define __COMPONENT_CONSOLE__

#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentConsole : public BaseComponent
		{
		  public:
			ComponentConsole( bool * );

		  protected:
			virtual void _draw() override;

		  private:
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
