#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_SCENE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_SCENE__

#include "contextual_menu_template.hpp"
#include <app/application/scene.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuScene : public ContextualMenuTemplate<App::Application::Scene>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuScene();
		void localize() override;

	  protected:
		ContextualMenuScene( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		void _loadMoleculeAction();
		void _downloadMoleculeAction();
		void _saveSessionAction();
		void _saveAsSessionAction();
		void _showAllMoleculesAction() const;
		void _addViewpointAction();

		void _clearAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
