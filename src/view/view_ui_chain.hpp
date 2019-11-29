#ifndef __VTX_VIEW_UI_CHAIN__
#define __VTX_VIEW_UI_CHAIN__

#include "../model/model_chain.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIChain : public BaseViewUI<Model::ModelChain>
		{
		  public:
			ViewUIChain( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
