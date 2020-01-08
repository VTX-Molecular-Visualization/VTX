#ifndef __VTX_VIEW_UI_RESIDUE__
#define __VTX_VIEW_UI_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_residue.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIResidue : public BaseViewUI<Model::ModelResidue>
		{
		  public:
			explicit ViewUIResidue( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

			virtual UI::COMPONENT_NAME getName() const override { return UI::COMPONENT_NAME::VIEW_RESIDUE; }

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
