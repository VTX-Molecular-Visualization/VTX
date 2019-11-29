#ifndef __VTX_VIEW_UI_ATOM__
#define __VTX_VIEW_UI_ATOM__

#include "../model/model_atom.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIAtom : public BaseViewUI<Model::ModelAtom>
		{
		  public:
			ViewUIAtom( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
