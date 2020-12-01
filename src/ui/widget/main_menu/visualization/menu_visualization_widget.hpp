#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_visualization_camera_action_widget.hpp"
#include "menu_visualization_object_display_widget.hpp"
#include "menu_visualization_render_effects_widget.hpp"
#include "menu_visualization_representation_widget.hpp"
#include "menu_visualization_selection_action_widget.hpp"
#include "menu_visualization_windows_widget.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					class MenuVisualizationWidget : public MenuTooltabWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationWidget();
						void localize() override;

					  protected:
						MenuVisualizationWidget( QWidget * p_parent = nullptr );
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						MenuVisualizationCameraActionWidget *	 _cameraAction	  = nullptr;
						MenuVisualizationRepresentationWidget *	 _representation  = nullptr;
						MenuVisualizationObjectDisplayWidget *	 _objectDisplay	  = nullptr;
						MenuVisualizationRenderEffectsWidget *	 _renderEffects	  = nullptr;
						MenuVisualizationSelectionActionWidget * _selectionAction = nullptr;
						MenuVisualizationWindowsWidget *		 _windows		  = nullptr;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
