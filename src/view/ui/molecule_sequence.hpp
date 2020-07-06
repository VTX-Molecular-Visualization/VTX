#ifndef __VTX_VIEW_UI_MOLECULE_SEQUENCE__
#define __VTX_VIEW_UI_MOLECULE_SEQUENCE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "view/base_view_ui_windows.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class MoleculeSequence : public BaseViewUIWindows<Model::Molecule>
			{
			  public:
				explicit MoleculeSequence( Model::Molecule * const p_model ) : BaseViewUIWindows( p_model ) {}

				virtual const char *		getTitle() const override { return LOCALE( "Sequence" ); }
				virtual const std::string & getName() const override { return ID::View::UI_MOLECULE_SEQUENCE; }
				virtual const std::string & getComponentParentName() const override { return ID::UI::USER_INTERFACE; };

			  protected:
				virtual void _drawContent() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
