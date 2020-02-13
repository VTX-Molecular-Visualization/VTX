#ifndef __VTX_VIEW_UI_RESIDUE__
#define __VTX_VIEW_UI_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Residue : public BaseViewUI<Model::Residue>
			{
			  public:
				explicit Residue( Model::Residue * const p_model ) : BaseViewUI( p_model ) {}

				virtual std::string getName() const override { return ID::View::UI_RESIDUE; };
				virtual std::string getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
