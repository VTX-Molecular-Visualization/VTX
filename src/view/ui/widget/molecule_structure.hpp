#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "ui/widget/base_manual_widget.hpp"
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
				class MoleculeStructure : public VTX::UI::Widget::BaseManualWidget<QTreeWidget>, public View::BaseView<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					MoleculeStructure( Model::Molecule * const, QWidget * = 0 );
					~MoleculeStructure();

					void localize() override;
					void notify( const Event::VTX_EVENT_MODEL & ) override;

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

				  private:
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
