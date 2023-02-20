#ifndef __VTX_UI_WIDGET_MENU_CAMERA__
#define __VTX_UI_WIDGET_MENU_CAMERA__

#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_camera_action_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_render_effects_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_snapshot.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_viewpoint.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_windows_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class MenuCameraWidget : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~MenuCameraWidget();
		void localize() override;

	  protected:
		MenuCameraWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		Visualization::MenuVisualizationCameraActionWidget *  _cameraAction	 = nullptr;
		Visualization::MenuVisualizationRenderEffectsWidget * _renderEffects = nullptr;
		Visualization::MenuVisualizationViewpointWidget *	  _viewpoints	 = nullptr;
		Visualization::MenuVisualizationSnapshot *			  _snapshot		 = nullptr;
		Visualization::MenuVisualizationWindowsWidget *		  _windows		 = nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
