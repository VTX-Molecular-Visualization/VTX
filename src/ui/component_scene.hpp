#ifndef __VTX_COMPONENT_SCENE__
#define __VTX_COMPONENT_SCENE__

#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../model/model_molecule.hpp"
#include "../view/base_view_ui.hpp"

namespace VTX
{
	namespace UI
	{
		class ComponentScene : public View::BaseViewUI<Model::ModelMolecule>
		{
		  public:
			ComponentScene( bool * const );
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace UI
} // namespace VTX
#endif
