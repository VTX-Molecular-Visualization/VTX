#ifndef __VTX_UI_TOOL_DEFAULT_SCENE_WINDOW__
#define __VTX_UI_TOOL_DEFAULT_SCENE_WINDOW__

#include "__new_archi/ui/core/tool_registration.hpp"
#include "__new_archi/ui/qt/widget/scene/scene_item_widget_instancier.hpp"
#include "__new_archi/ui/qt/widget/scene/scene_widget.hpp"
#include "keys.hpp"
#include "object3d/scene.hpp"

namespace VTX::UI::DefaultTools
{
	class SceneWindow : public VTX::UI::Core::BaseVTXUITool
	{
		REGISTER_TOOL( SceneWindow, SCENE_WINDOW_KEY );

		class MoleculeInstancier : public QT::Widget::Scene::SceneItemWidgetInstancier
		{
		  public:
			QT::Widget::Scene::SceneItemWidget * instantiateItem( Generic::BaseSceneItem * const p_item ) override;
			void destroyItem( const QT::Widget::Scene::SceneItemWidget * p_sceneWidget ) override;
		};

		class PathInstancier : public QT::Widget::Scene::SceneItemWidgetInstancier
		{
		  public:
			QT::Widget::Scene::SceneItemWidget * instantiateItem( Generic::BaseSceneItem * const p_item ) override;
			void destroyItem( const QT::Widget::Scene::SceneItemWidget * p_sceneWidget ) override;
		};

	  public:
		SceneWindow();

		void instantiateTool() override;
		void setupContextualMenu() const;

	  private:
		void _registerDefaultObjects( QT::Widget::Scene::SceneWidget & p_sceneWidget ) const;
		void _instantiateObjectsInScene( QT::Widget::Scene::SceneWidget & p_sceneWidget,
										 const VTX::Object3D::Scene &	  p_scene ) const;
	};
} // namespace VTX::UI::DefaultTools

#endif
