#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_visualization_render_effect_button_widget.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationRenderEffectsWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuVisualizationRenderEffectsWidget();
		void localize() override;

	  protected:
		MenuVisualizationRenderEffectsWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		std::vector<RenderEffectPresetButton *> _presetButtons = std::vector<RenderEffectPresetButton *>();

		// Create Preset
		MenuToolButtonWidget * _createPreset = nullptr;

		// Render Feature
		MenuToolButtonWidget * _fullscreen	 = nullptr;
		MenuToolButtonWidget * _takeSnapshot = nullptr;

		// ACTIONS //
		void _takeSnapshotAction() const;
		void _openPresetSettings() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
