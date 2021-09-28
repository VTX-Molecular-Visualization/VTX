#ifndef __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET_LIBRARY__
#define __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET_LIBRARY__

#include "model/renderer/render_effect_preset_library.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/renderer/render_effect_library_combo_box.hpp"
#include "ui/widget/settings/render_effect_preset_editor.hpp"
#include "view/base_view.hpp"
#include <QComboBox>
#include <QPushButton>
#include <QWidget>

namespace VTX::View::UI::Widget::Renderer
{
	class RenderEffectPresetLibraryView :
		public View::BaseView<Model::Renderer::RenderEffectPresetLibrary>,
		public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

		using RenderEffectPresetEditor	  = VTX::UI::Widget::Settings::RenderEffectPresetEditor;
		using RenderEffectLibraryComboBox = VTX::UI::Widget::Renderer::RenderEffectLibraryComboBox;

	  public:
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void notify( const Event::VTXEvent * const p_event ) override;

		void selectPreset( const int p_presetIndex );

	  protected:
		RenderEffectPresetLibraryView( Model::Renderer::RenderEffectPresetLibrary * const p_model,
									   QWidget * const									  p_parent );

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _refreshView() override;

		void _onPresetIndexChanged( const int p_newIndex );
		void _onAddPreset() const;
		void _onCopyPreset() const;
		void _onDeletePreset();

		void _onImportPreset() const;
		void _onReloadLibrary() const;
		void _onResetToDefaultLibrary() const;

		void _refreshPresetDisplayed( const bool p_updateRenderer );
		void _refreshDeletePresetButton();

	  private:
		RenderEffectLibraryComboBox * _presetList		  = nullptr;
		QPushButton *				  _addPresetButton	  = nullptr;
		QPushButton *				  _copyPresetButton	  = nullptr;
		QPushButton *				  _deletePresetButton = nullptr;

		QPushButton * _importPresetButton = nullptr;
		QPushButton * _reloadButton		  = nullptr;
		QPushButton * _resetButton		  = nullptr;

		RenderEffectPresetEditor * _renderPresetEditor = nullptr;
	};

} // namespace VTX::View::UI::Widget::Renderer

#endif
