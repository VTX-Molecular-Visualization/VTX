#ifndef __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET_LIBRARY__
#define __VTX_VIEW_UI_WIDGET_RENDER_EFFECT_PRESET_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/renderer/render_effect_preset_library.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/settings/render_effect_preset_editor.hpp"
#include "view/base_view.hpp"
#include <QTabWidget>
#include <vector>

namespace VTX::View::UI::Widget::Renderer
{
	class RenderEffectPresetLibraryView :
		public View::BaseView<Model::Renderer::RenderEffectPresetLibrary>,
		public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

		using RenderEffectPresetEditor = VTX::UI::Widget::Settings::RenderEffectPresetEditor;

	  public:
		void localize() override;
		void notify( const Event::VTXEvent * const p_event ) override;

	  protected:
		RenderEffectPresetLibraryView( Model::Renderer::RenderEffectPresetLibrary * const p_model,
									   QWidget * const									  p_parent );

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _refreshView() override;

		void _onPresetIndexChanged( const int p_newIndex );
		void _refreshPresetDisplayed();
		void _buildPresetList();

	  private:
		QComboBox *				   _presetList;
		RenderEffectPresetEditor * _renderPresetEditor;
	};

} // namespace VTX::View::UI::Widget::Renderer

#endif