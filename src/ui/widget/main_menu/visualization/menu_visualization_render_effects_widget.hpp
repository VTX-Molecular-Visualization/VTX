#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "menu_visualization_render_effect_button_widget.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>
#include <vector>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationRenderEffectsWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		MenuVisualizationRenderEffectsWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		std::vector<RenderEffectPresetButton *> _presetButtons = std::vector<RenderEffectPresetButton *>();

		// Create Preset
		MenuToolButtonWidget * _createPreset = nullptr;

		// Render Feature
		MenuToolButtonWidget * _fullscreen	 = nullptr;
		MenuToolButtonWidget * _takeSnapshot = nullptr;

		void _updateFullscreenButton( const WindowMode & p_mode );

		// ACTIONS //
		void _takeSnapshotAction() const;
		void _openPresetSettings() const;
		void _toggleWindowState() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
