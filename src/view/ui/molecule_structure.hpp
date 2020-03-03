#ifndef __VTX_VIEW_UI_MOLECULE_STRUCTURE__
#define __VTX_VIEW_UI_MOLECULE_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "view/base_view_ui.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			class MoleculeStructure : public BaseViewUI<Model::Molecule>
			{
			  public:
				explicit MoleculeStructure( Model::Molecule * const p_model ) : BaseViewUI( p_model ) {}

				virtual const std::string & getName() const override { return ID::View::UI_MOLECULE_STRUCTURE; }
				virtual std::string getComponentParentName() const override { return ID::UI::SCENE; };

			  protected:
				virtual void _draw() override;

			  private:
			};
		} // namespace UI
	}	  // namespace View
} // namespace VTX
#endif
