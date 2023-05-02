#include "ui/old_ui/ui/widget/main_menu/camera/render_effect_preset_button.hpp"
#include "ui/old_ui/style.hpp"

#include <app/action/renderer.hpp>
#include <app/application/render_effect/render_effect_preset.hpp>
#include <app/application/render_effect/render_effect_library.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	void RenderEffectPresetButton::_setupUi( const QString & p_name )
	{
		MenuToolButtonWidget::_setupUi( p_name );
		setMaximumWidth( Style::MAIN_MENU_MAX_BUTTON_PRESET_WIDTH );
	}

	void RenderEffectPresetButton::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		connect( this, &QToolButton::clicked, this, &RenderEffectPresetButton::_onButtonClicked );
	}

	void RenderEffectPresetButton::_onButtonClicked()
	{
		VTX::App::Application::RenderEffect::RenderEffectPreset * renderEffectPreset
			= VTX::App::Application::RenderEffect::RenderEffectLibrary::get().getPreset( _id );

		VTX_ACTION( new App::Action::Renderer::ApplyRenderEffectPreset( *renderEffectPreset, true ) );
	};

} // namespace VTX::UI::Widget::MainMenu::Camera
