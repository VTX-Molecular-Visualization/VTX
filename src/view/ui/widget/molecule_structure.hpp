#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "ui/widget/base_widget.hpp"
#include "ui_molecule_structure.h"
#include "view/base_view.hpp"
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeStructure : public VTX::UI::Widget::BaseWidget<QTreeWidget, Ui_MoleculeStructure>, public View::BaseView<Model::Molecule>
				{
					Q_OBJECT

				  public:
					MoleculeStructure( Model::Molecule * const, QWidget * = 0 );

					void notify( const Event::VTX_EVENT_MODEL & ) override;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
			  // namespace UI
} // namespace VTX

#endif
