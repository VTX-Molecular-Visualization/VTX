#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SEQUENCE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SEQUENCE_VIEW__

#include "base_widget_view.hpp"
#include "ui/old_ui/ui/widget/sequence/molecule_sequence_widget.hpp"
#include <app/old/component/chemistry/molecule.hpp>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSequenceView :
					public BaseWidgetView<App::Old::Component::Chemistry::Molecule, VTX::UI::Widget::Sequence::MoleculeSequenceWidget>
				{
					VTX_VIEW

				  private:
					MoleculeSequenceView( App::Old::Component::Chemistry::Molecule * const p_molecule, QWidget * p_parent ) :
						BaseWidgetView( p_molecule, "MoleculeSequence", p_parent ) {};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
