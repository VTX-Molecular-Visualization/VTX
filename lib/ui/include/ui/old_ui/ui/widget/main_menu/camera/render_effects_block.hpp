#ifndef __VTX_UI_WIDGET_MENU_CAMERA_RENDER_EFFECTS_BLOCK__
#define __VTX_UI_WIDGET_MENU_CAMERA_RENDER_EFFECTS_BLOCK__

#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "render_effect_preset_button.hpp"
#include <QWidget>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/application/render_effect/render_effect_library.hpp>
#include <app/old/core/view/base_view.hpp>
#include <vector>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class RenderEffectsBlock :
		public VTX::App::Old::Core::View::BaseView<App::Old::Application::RenderEffect::RenderEffectLibrary>,
		public MenuToolBlockWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;
		void localize() override;

		void notify( const VTX::App::Old::Core::Event::VTXEvent * const p_event ) override;

	  protected:
		RenderEffectsBlock( App::Old::Application::RenderEffect::RenderEffectLibrary * const _renderEffectLibrary,
							QWidget *										   p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshView() override;

	  private:
		std::vector<RenderEffectPresetButton *> _presetButtons = std::vector<RenderEffectPresetButton *>();

		// Create Preset
		MenuToolButtonWidget * _createPreset = nullptr;

		void _instantiateUI();

		// ACTIONS //
		void _takeSnapshotAction() const;
		void _exportAsImageAction() const;
		void _openPresetSettings() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
