#include "setting_render_effect_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QString>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Settings
{
	SettingRenderEffectWidget::SettingRenderEffectWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent ) {}

	void SettingRenderEffectWidget::receiveEvent( const Event::VTXEvent & p_event ) {}

	void SettingRenderEffectWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_presetLibrary
			= WidgetFactory::get().instantiateViewWidget<View::UI::Widget::Renderer::RenderEffectPresetLibraryView>(
				&Model::Renderer::RenderEffectPresetLibrary::get(),
				ID::View::UI_RENDER_EFFECT_PRESET_LIBRARY,
				this,
				"PresetLibrary" );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->addWidget( _presetLibrary );
	}

	void SettingRenderEffectWidget::_setupSlots() {};
	void SettingRenderEffectWidget::localize() {};

	void SettingRenderEffectWidget::showEvent( QShowEvent * p_event )
	{
		BaseManualWidget::showEvent( p_event );

		const int currentPresetUsed = Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPresetIndex();
		_presetLibrary->selectPreset( currentPresetUsed );
	}

} // namespace VTX::UI::Widget::Settings
