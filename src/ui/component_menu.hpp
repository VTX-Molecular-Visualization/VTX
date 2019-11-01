#ifndef __COMPONENT_MENU__
#define __COMPONENT_MENU__

#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentMenu : public BaseComponent
		{
		  public:
			ComponentMenu( bool *, bool * );

		  protected:
			virtual void _draw() override;

		  private:
			bool * _showConsole;
		};
	} // namespace UI
} // namespace VTX
#endif
