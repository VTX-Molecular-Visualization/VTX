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
		class ComponentSelection : public BaseComponent, public View::BaseView<Model::ModelMolecule>
		{
		  public:
			ComponentSelection( bool * const );
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
