#ifndef __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM_VIEW__
#define __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/residue.hpp"
#include "ui/widget/scene_tree_widget_item.hpp"
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class MoleculeSubDataSceneView : public VTX::UI::Widget::SceneTreeWidgetItem
				{
				  public:
					MoleculeSubDataSceneView( Model::BaseModel & p_model, QTreeWidgetItem * p_parent ) : SceneTreeWidgetItem( p_model, p_parent ) {};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
