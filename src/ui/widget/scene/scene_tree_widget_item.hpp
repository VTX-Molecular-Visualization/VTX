#ifndef __VTX_UI_WIDGET_SCENE_TREE_ITEM__
#define __VTX_UI_WIDGET_SCENE_TREE_ITEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include <QIcon>
#include <QTreeWidgetItem>
#include <string>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
			{
				class SceneTreeWidgetItem : public QTreeWidgetItem
				{
				  public:
					SceneTreeWidgetItem( Model::BaseModel & p_model, QTreeWidgetItem * const p_parent ) : QTreeWidgetItem( p_parent ), _linkedModel( &p_model ) { refreshItem(); };

					Model::BaseModel * const getModel() { return _linkedModel; };

					virtual void refreshItem();

				  protected:
					const Qt::CheckState getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };

					Model::BaseModel * const _linkedModel = nullptr;
				};
			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
