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
			class SceneTreeWidgetItem : public QTreeWidgetItem
			{
			  public:
				SceneTreeWidgetItem( Model::BaseModel & p_model, QTreeWidgetItem * p_parent ) : QTreeWidgetItem( p_parent ), _linkedModel( &p_model ) {};
				void fillData( const Model::BaseModel & p_model, const std::string & p_name, const QIcon & p_icon );

				Model::BaseModel * const getModel() { return _linkedModel; };

			  protected:
				Qt::CheckState			 getCheckState( const Model::BaseModel & p_model );
				Model::BaseModel * const _linkedModel = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
