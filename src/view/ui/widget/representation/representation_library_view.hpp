#ifndef __VTX_VIEW_UI_WIDGET_REPRESENTATION_LIBRARY__
#define __VTX_VIEW_UI_WIDGET_REPRESENTATION_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/representation_library.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/representation/base_representation_widget.hpp"
#include "ui/widget/representation/representation_library_combo_box.hpp"
#include "ui/widget/settings/representation_preset_editor.hpp"
#include "view/base_view.hpp"
#include <QWidget>

namespace VTX::View::UI::Widget::Representation
{
	class RepresentationLibraryView :
		public View::BaseView<Model::Representation::RepresentationLibrary>,
		public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

	  private:
		using RepresentationPresetEditor	= VTX::UI::Widget::Settings::RepresentationPresetEditor;
		using RepresentationLibraryComboBox = VTX::UI::Widget::Representation::RepresentationLibraryComboBox;

	  public:
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		RepresentationLibraryView( Model::Representation::RepresentationLibrary * const p_model,
								   QWidget * const										p_parent );

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _refreshView() override;

		void _onPresetIndexChanged( const int p_newIndex );
		void _onAddPreset() const;
		void _onCopyPreset() const;
		void _onDeletePreset() const;

		void _refreshPresetDisplayed( const bool p_applyPreset );

	  private:
		RepresentationLibraryComboBox * _presetList					= nullptr;
		RepresentationPresetEditor *	_representationPresetEditor = nullptr;

		QPushButton * _addPresetButton	  = nullptr;
		QPushButton * _copyPresetButton	  = nullptr;
		QPushButton * _deletePresetButton = nullptr;
	};
} // namespace VTX::View::UI::Widget::Representation
#endif
