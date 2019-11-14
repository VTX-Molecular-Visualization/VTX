#ifndef __VTX_COMPONENT_DIALOG_IMPORT__
#define __VTX_COMPONENT_DIALOG_IMPORT__

#include "../base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentDialogImport : public BaseComponent
		{
		  public:
			ComponentDialogImport( bool * const );

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
