#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QTreeWidget>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class SceneTreeWidget : public QT::Widget::BaseManualWidget<QTreeWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		SceneTreeWidget( QWidget * p_parent = nullptr );
		~SceneTreeWidget() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
