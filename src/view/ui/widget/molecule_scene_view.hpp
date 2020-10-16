#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_SCENE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_scene_item.hpp"
#include "model/molecule.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSceneView : public BaseSceneItem<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void localize() override;

				  protected:
					MoleculeSceneView( Model::Molecule * const, QTreeWidgetItem * );
					void _setupUi( const QString & ) override;
					void _setupSlots() override;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
