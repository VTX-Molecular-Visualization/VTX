#ifndef __VTX_UI_TOOL_DEFAULT_SCENE_WINDOW__
#define __VTX_UI_TOOL_DEFAULT_SCENE_WINDOW__

#include "ui/core/tool_registration.hpp"
#include "keys.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/tool/scene/widget/scene_item_widget_instancier.hpp"
#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include <app/old/application/scene.hpp>

namespace VTX::UI::QT::Tool
{
	class SceneWindow : public BaseQtTool
	{
		REGISTER_TOOL( SceneWindow, SCENE_WINDOW_KEY );

		class MoleculeInstancier : public Scene::Widget::SceneItemWidgetInstancier
		{
		  public:
			Scene::Widget::SceneItemWidget * instantiateItem( App::Old::Core::Scene::BaseSceneItem * const p_item ) override;
			void destroyItem( const Scene::Widget::SceneItemWidget * p_sceneWidget ) override;
		};

		class PathInstancier : public Scene::Widget::SceneItemWidgetInstancier
		{
		  public:
			Scene::Widget::SceneItemWidget * instantiateItem( App::Old::Core::Scene::BaseSceneItem * const p_item ) override;
			void destroyItem( const Scene::Widget::SceneItemWidget * p_sceneWidget ) override;
		};

	  public:
		SceneWindow();

		void instantiateTool() override;
		void setupContextualMenu() const;

	  private:
		void _registerDefaultObjects( Scene::Widget::SceneWidget & p_sceneWidget ) const;
		void _instantiateObjectsInScene( Scene::Widget::SceneWidget & p_sceneWidget,
										 const VTX::App::Old::Application::Scene & p_scene ) const;
	};
} // namespace VTX::UI::QT::Tool

#endif
