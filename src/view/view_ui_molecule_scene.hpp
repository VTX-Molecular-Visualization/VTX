#ifndef __VTX_VIEW_UI_MOLECULE_SCENE__
#define __VTX_VIEW_UI_MOLECULE_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIMoleculeScene : public BaseViewUI<Model::ModelMolecule>
		{
		  public:
			explicit ViewUIMoleculeScene( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

			virtual UI::COMPONENT_NAME getName() const override { return UI::COMPONENT_NAME::VIEW_MOLECULE_SCENE; }

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
