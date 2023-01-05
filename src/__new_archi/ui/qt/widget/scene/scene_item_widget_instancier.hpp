#ifndef __VTX_UI_QT_WIDGET_SCENE_ITEM_WIDGET_INSTANTIER__
#define __VTX_UI_QT_WIDGET_SCENE_ITEM_WIDGET_INSTANTIER__

#include "__new_archi/ui/qt/widget/scene/scene_item_widget.hpp"
#include "generic/base_scene_item.hpp"

namespace VTX::UI::QT::Widget::Scene
{
	class SceneItemWidgetInstancier
	{
	  public:
		virtual SceneItemWidget * instantiateItem( Generic::BaseSceneItem * const p_model ) = 0;
		virtual void			  destroyItem( const SceneItemWidget * p_item )				= 0;
	};

} // namespace VTX::UI::QT::Widget::Scene

#endif
