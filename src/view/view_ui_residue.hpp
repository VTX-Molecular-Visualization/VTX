#ifndef __VTX_VIEW_UI_RESIDUE__
#define __VTX_VIEW_UI_RESIDUE__

#include "../model/model_residue.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIResidue : public BaseViewUI<Model::ModelResidue>
		{
		  public:
			ViewUIResidue( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
