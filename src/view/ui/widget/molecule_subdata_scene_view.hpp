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
					MoleculeSubDataSceneView( QTreeWidgetItem * p_parent ) : SceneTreeWidgetItem( p_parent ) {};
					void setData( const Model::Chain & p_chain );
					void setData( const Model::Residue & p_residue );
					void setData( const Model::Atom & p_atom );
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
