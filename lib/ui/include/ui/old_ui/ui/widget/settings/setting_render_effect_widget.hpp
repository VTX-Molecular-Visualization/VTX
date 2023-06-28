#ifndef __VTX_UI_WIDGET_SETTING_RENDER_EFFECT__
#define __VTX_UI_WIDGET_SETTING_RENDER_EFFECT__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/view/ui/widget/renderer/render_effect_preset_library_view.hpp"
#include <QShowEvent>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingRenderEffectWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingRenderEffectWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void showEvent( QShowEvent * p_event ) override;

	  private:
		View::UI::Widget::Renderer::RenderEffectPresetLibraryView * _presetLibrary;
	};
} // namespace VTX::UI::Widget::Settings
#endif
