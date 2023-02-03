#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM_WIDGET_INSTANTIER__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM_WIDGET_INSTANTIER__

#include "qt/tool/scene/widget/scene_item_widget.hpp"
#include "src/generic/base_scene_item.hpp"

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class SceneItemWidgetInstancier
	{
	  public:
		virtual SceneItemWidget * instantiateItem( Generic::BaseSceneItem * const p_model ) = 0;
		virtual void			  destroyItem( const SceneItemWidget * p_item )				= 0;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
