#ifndef __VTX_VIEW_UI_CHAIN__
#define __VTX_VIEW_UI_CHAIN__

#ifdef _MSC_VER
#pragma once

#endif
#include "model/chain.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class Chain : public BaseViewUI<Model::Chain>
			{
			  public:
				explicit Chain( Model::Chain * const p_model ) : BaseViewUI( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::UI_CHAIN; };
				virtual std::string			getComponentParentName() const override { return ID::UI::INSPECTOR; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
