#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_CONTEXTUAL_MENU_SCENE__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_CONTEXTUAL_MENU_SCENE__

#include "ui/qt/widget/base_manual_widget.hpp"
#include "ui/qt/widget/contextual_menu/contextual_menu_template.hpp"
#include <QMenu>
#include <app/old/application/scene.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class ContextualMenuScene : public QT::Widget::ContextualMenu::ContextualMenuTemplate<App::Old::Application::Scene>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~ContextualMenuScene();
		void localize() override;

	  protected:
		ContextualMenuScene( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
