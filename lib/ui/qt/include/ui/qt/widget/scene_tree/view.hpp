#ifndef __VTX_UI_QT_WIDGET_SCENE_TREE_VIEW__
#define __VTX_UI_QT_WIDGET_SCENE_TREE_VIEW__

#include <QTreeView>

namespace VTX::UI::QT::Widget::SceneTree
{
	class View : public QTreeView
	{
	  public:
		View( QWidget * p_parent = nullptr );
	};
} // namespace VTX::UI::QT::Widget::SceneTree

#endif
