#ifndef __VTX_COMPONENT_MENU__
#define __VTX_COMPONENT_MENU__

#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentMenu : public BaseComponent
		{
		  public:
			ComponentMenu( bool * const, bool * const, bool * const, bool * const );

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
			bool * const _showConsole;
			bool * const _showScene;
			bool * const _showSelection;
			bool		 _showDialogImport = false;
		};
	} // namespace UI
} // namespace VTX
#endif
