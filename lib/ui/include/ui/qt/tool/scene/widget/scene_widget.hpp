#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_WIDGET__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_WIDGET__

#include "scene_tree_widget.hpp"
#include "ui/qt/qt_panel.hpp"

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class SceneWidget : public QtDockablePanel
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		SceneWidget( QWidget * p_parent = nullptr );
		~SceneWidget() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		SceneTreeWidget * _treeWidget;


	};
} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
