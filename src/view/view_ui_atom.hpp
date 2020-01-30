#ifndef __VTX_VIEW_UI_ATOM__
#define __VTX_VIEW_UI_ATOM__

#ifdef _MSC_VER
#pragma once

#endif
#include "../model/model_atom.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIAtom : public BaseViewUI<Model::ModelAtom>
		{
		  public:
			explicit ViewUIAtom( Model::ModelAtom * const p_model ) : BaseViewUI( p_model ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

			virtual std::string getName() const override { return ID::View::UI_ATOM; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
