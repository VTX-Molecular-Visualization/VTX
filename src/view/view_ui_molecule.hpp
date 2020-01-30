#ifndef __VTX_VIEW_UI_MOLECULE__
#define __VTX_VIEW_UI_MOLECULE__

#ifdef _MSC_VER
#pragma once

#endif
#include "../model/model_molecule.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIMolecule : public BaseViewUI<Model::ModelMolecule>
		{
		  public:
			explicit ViewUIMolecule( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

			virtual std::string getName() const override { return ID::View::UI_MOLECULE; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
