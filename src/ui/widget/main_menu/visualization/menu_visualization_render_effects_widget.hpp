#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECTS__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "menu_visualization_render_effect_button_widget.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <vector>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationRenderEffectsWidget :
		public VTX::View::BaseView<Model::Renderer::RenderEffectPresetLibrary>,
		public MenuToolBlockWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

		void notify( const Event::VTXEvent * const p_event ) override;

	  protected:
		MenuVisualizationRenderEffectsWidget( Model::Renderer::RenderEffectPresetLibrary * const _renderEffectLibrary,
											  QWidget *											 p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshView() override;

	  private:
		std::vector<RenderEffectPresetButton *> _presetButtons = std::vector<RenderEffectPresetButton *>();

		// Create Preset
		MenuToolButtonWidget * _createPreset = nullptr;

		// Render Feature
		MenuToolButtonWidget * _fullscreen	 = nullptr;
		MenuToolButtonWidget * _takeSnapshot = nullptr;

		void _instantiateUI();

		void _updateFullscreenButton( const WindowMode & p_mode );

		// ACTIONS //
		void _takeSnapshotAction() const;
		void _openPresetSettings() const;
		void _toggleWindowState() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
