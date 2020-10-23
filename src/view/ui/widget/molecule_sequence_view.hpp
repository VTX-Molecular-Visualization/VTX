#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SEQUENCE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SEQUENCE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget_view.hpp"
#include "model/molecule.hpp"
#include "ui/widget/sequence/molecule_sequence_widget.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSequenceView : public BaseWidgetView<Model::Molecule, VTX::UI::Widget::Sequence::MoleculeSequenceWidget>
				{
				  public:
					MoleculeSequenceView( Model::Molecule * const p_molecule, QWidget * p_parent ) : BaseWidgetView( p_molecule, "MoleculeSequence", p_parent ) {};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
