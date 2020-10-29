#ifndef __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM__
#define __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "ui/widget/scene/scene_tree_widget_item.hpp"
#include "view/templated_base_view.hpp"
#include <QTreeWidget>
#include <type_traits>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
				class BaseSceneItem : public QTreeWidgetItem, public View::TemplatedBaseView<M>, VTX::UI::Widget::BaseManualWidgetInitializer
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  protected:
					BaseSceneItem( M * const p_model, QTreeWidgetItem * p_parent ) :
						View::TemplatedBaseView<M>( p_model ), QTreeWidgetItem( p_parent ), BaseManualWidgetInitializer() {};
					inline virtual void _setupUi( const QString & p_name ) override {};

					inline virtual void refreshView() override { View::TemplatedBaseView<M>::refreshView(); };
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
