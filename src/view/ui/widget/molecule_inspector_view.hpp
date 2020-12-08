#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget_view.hpp"
#include "model/molecule.hpp"
#include "ui/widget/inspector/inspector_molecule_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeInspectorView : public BaseWidgetView<Model::Molecule, VTX::UI::Widget::Inspector::InspectorMoleculeWidget>
				{
					VTX_VIEW

				  private:
					MoleculeInspectorView( Model::Molecule * const p_molecule ) : BaseWidgetView( p_molecule, "MoleculeInspector" ) {};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
