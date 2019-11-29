#ifndef __VTX_COMPONENT_SELECTION__
#define __VTX_COMPONENT_SELECTION__

#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../model/model_molecule.hpp"
#include "../view/base_view.hpp"
#include "base_component.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentSelection : public BaseComponent
		{
		  public:
			ComponentSelection( bool * const );

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
