#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_molecule_structure.h"
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeStructure : public QTreeWidget
				{
					Q_OBJECT

				  public:
					MoleculeStructure( QWidget * = 0 );
					~MoleculeStructure();

				  private:
					Ui_MoleculeStructure * _ui;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
			  // namespace UI
} // namespace VTX

#endif
