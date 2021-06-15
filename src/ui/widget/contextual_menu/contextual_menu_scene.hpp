#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_SCENE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_SCENE__

#include "contextual_menu_template.hpp"
#include "object3d/scene.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuScene : public ContextualMenuTemplate<Object3D::Scene>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuScene();
		void localize() override;

	  protected:
		ContextualMenuScene( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _loadMoleculeAction();
		void _downloadMoleculeAction();
		void _clearAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
