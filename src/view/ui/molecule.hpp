#ifndef __VTX_VIEW_UI_MOLECULE__
#define __VTX_VIEW_UI_MOLECULE__

#ifdef _MSC_VER
#pragma once

#endif
#include "model/molecule.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Molecule : public BaseViewUI<Model::Molecule>
			{
			  public:
				explicit Molecule( Model::Molecule * const p_model ) : BaseViewUI( p_model ) {}

				virtual std::string getName() const override { return ID::View::UI_MOLECULE; };
				virtual std::string getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
