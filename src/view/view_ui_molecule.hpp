#ifndef __VTX_VIEW_UI_MOLECULE__
#define __VTX_VIEW_UI_MOLECULE__

#include "../model/model_molecule.hpp"
#include "base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		class ViewUIMolecule : public BaseViewUI<Model::ModelMolecule>
		{
		  public:
			ViewUIMolecule( bool * const p_show ) : BaseViewUI( p_show ) {}
			virtual void notify( Event::EVENT_MODEL ) override;

		  protected:
			virtual void _draw() override;

		  private:
		};
	} // namespace View
} // namespace VTX
#endif
