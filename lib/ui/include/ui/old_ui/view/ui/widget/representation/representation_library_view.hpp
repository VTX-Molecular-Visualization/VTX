#ifndef __VTX_VIEW_UI_WIDGET_REPRESENTATION_LIBRARY__
#define __VTX_VIEW_UI_WIDGET_REPRESENTATION_LIBRARY__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/representation/base_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/representation_library_combo_box.hpp"
#include "ui/old_ui/ui/widget/settings/representation_preset_editor.hpp"
#include <QWidget>
#include <app/old/application/representation/representation_library.hpp>
#include <app/old/core/view/base_view.hpp>

namespace VTX::View::UI::Widget::Representation
{
	class RepresentationLibraryView :
		public App::Old::Core::View::BaseView<App::Old::Application::Representation::RepresentationLibrary>,
		public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

	  private:
		using RepresentationPresetEditor	= VTX::UI::Widget::Settings::RepresentationPresetEditor;
		using RepresentationLibraryComboBox = VTX::UI::Widget::Representation::RepresentationLibraryComboBox;

	  public:
		void localize() override;
		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

	  protected:
		RepresentationLibraryView( App::Old::Application::Representation::RepresentationLibrary * const p_model,
								   QWidget * const										p_parent );

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _refreshView() override;

		void _onPresetIndexChanged( const int p_newIndex );
		void _onAddPreset() const;
		void _onCopyPreset() const;
		void _onDeletePreset();

		void _onImportPreset() const;
		void _onReloadLibrary() const;
		void _onResetLibrary() const;

		void _refreshPresetDisplayed( const bool p_applyPreset );

	  private:
		RepresentationLibraryComboBox * _presetList					= nullptr;
		RepresentationPresetEditor *	_representationPresetEditor = nullptr;

		QPushButton * _addPresetButton	  = nullptr;
		QPushButton * _copyPresetButton	  = nullptr;
		QPushButton * _deletePresetButton = nullptr;

		QPushButton * _importPresetButton  = nullptr;
		QPushButton * _reloadLibraryButton = nullptr;
		QPushButton * _resetLibraryButton  = nullptr;
	};
} // namespace VTX::View::UI::Widget::Representation
#endif
