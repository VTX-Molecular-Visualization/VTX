#ifndef __COMPONENT_MENU__
#define __COMPONENT_MENU__

#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentMenu : public BaseComponent
		{
		  protected:
			virtual void _draw() override;
		};
	} // namespace UI
} // namespace VTX
#endif
