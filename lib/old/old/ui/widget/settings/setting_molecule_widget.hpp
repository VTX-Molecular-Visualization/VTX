#ifndef __VTX_UI_WIDGET_SETTINGS_MOLECULE__
#define __VTX_UI_WIDGET_SETTINGS_MOLECULE__

#include "io/struct/image_export.hpp"
#include "ui/layout/attribute_list_layout.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/representation/representation_library_combo_box.hpp"
#include <QComboBox>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingMoleculeWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

		using RepresentationLibraryComboBox = VTX::UI::Widget::Representation::RepresentationLibraryComboBox;

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingMoleculeWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshData();

		void _refreshRepresentationComboBox( RepresentationLibraryComboBox * const p_comboBox,
											 const CATEGORY_ENUM				   p_categoryEnum );

	  private:
		Layout::AttributeListLayout * _layout;

		RepresentationLibraryComboBox * _defaultRepresentationForPolymer	  = nullptr;
		RepresentationLibraryComboBox * _defaultRepresentationForCarbohydrate = nullptr;
		RepresentationLibraryComboBox * _defaultRepresentationForLigand		  = nullptr;
		RepresentationLibraryComboBox * _defaultRepresentationForIon		  = nullptr;
		RepresentationLibraryComboBox * _defaultRepresentationForSolvent	  = nullptr;
		RepresentationLibraryComboBox * _defaultRepresentationForWater		  = nullptr;

		QPushButton * _restoreButton = nullptr;

		void _defaultRepresentationForPolymerChange( int p_index );
		void _defaultRepresentationForCarbohydrateChange( int p_index );
		void _defaultRepresentationForLigandChange( int p_index );
		void _defaultRepresentationForIonChange( int p_index );
		void _defaultRepresentationForSolventChange( int p_index );
		void _defaultRepresentationForWaterChange( int p_index );

		void _restoreDefaultRepresentations();
	};
} // namespace VTX::UI::Widget::Settings
#endif
