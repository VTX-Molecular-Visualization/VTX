#ifndef __VTX_VIEW_UI_ATOM__
#define __VTX_VIEW_UI_ATOM__

#ifdef _MSC_VER
#pragma once

#endif
#include "../base_view_ui.hpp"
#include "model/atom.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Atom : public BaseViewUI<Model::Atom>
			{
			  public:
				explicit Atom( Model::Atom * const p_model ) : BaseViewUI( p_model ) {}

				virtual std::string getName() const override { return ID::View::UI_ATOM; };
				virtual std::string getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
