#ifndef __COMPONENT_CONSOLE__
#define __COMPONENT_CONSOLE__

#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentConsole : public BaseComponent
		{
		  protected:
			virtual void _draw() override;

		  private:
			void _clear();
		};
	} // namespace UI
} // namespace VTX
#endif
