#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_CONTEXTUAL_MENU_SCENE__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_CONTEXTUAL_MENU_SCENE__

#include "qt/widget/base_manual_widget.hpp"
#include "qt/widget/contextual_menu/contextual_menu_template.hpp"
#include <old/object3d/scene.hpp>
#include <QMenu>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class ContextualMenuScene : public QT::Widget::ContextualMenu::ContextualMenuTemplate<Object3D::Scene>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~ContextualMenuScene();
		void localize() override;

	  protected:
		ContextualMenuScene( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _loadMoleculeAction();
		void _downloadMoleculeAction();
		void _showAllMoleculesAction() const;
		void _addViewpointAction();

		void _clearAction();
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif