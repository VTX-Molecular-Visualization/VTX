#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
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
					class MenuVisualizationRenderEffectsWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationRenderEffectsWidget();
						void localize() override;

					  protected:
						MenuVisualizationRenderEffectsWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Predefined Preset list
						MenuToolButtonWidget * _predefinedPreset1 = nullptr;
						MenuToolButtonWidget * _predefinedPreset2 = nullptr;
						MenuToolButtonWidget * _predefinedPreset3 = nullptr;

						// Custom Preset list
						MenuToolButtonWidget * _customPreset1 = nullptr;
						MenuToolButtonWidget * _customPreset2 = nullptr;
						MenuToolButtonWidget * _customPreset3 = nullptr;

						// Create Preset
						MenuToolButtonWidget * _createPreset = nullptr;

						// Render Feature
						MenuToolButtonWidget * _fullscreen	 = nullptr;
						MenuToolButtonWidget * _takeSnapshot = nullptr;

						// ACTIONS //
						void _takeSnapshotAction() const;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
