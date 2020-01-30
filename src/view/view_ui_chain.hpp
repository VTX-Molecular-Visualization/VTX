#ifndef __VTX_VIEW_UI_CHAIN__
#define __VTX_VIEW_UI_CHAIN__

#ifdef _MSC_VER
#pragma once

#endif
#include "../model/model_chain.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIChain : public BaseViewUI<Model::ModelChain>
		{
		  public:
			explicit ViewUIChain( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

			virtual std::string getName() const override { return ID::View::UI_CHAIN; };

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
