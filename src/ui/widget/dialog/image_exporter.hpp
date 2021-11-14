#ifndef __VTX_UI_DIALOG_IMAGE_EXPORTER__
#define __VTX_UI_DIALOG_IMAGE_EXPORTER__

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/collapsing_groupbox_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLayout>
#include <QShowEvent>

namespace VTX::UI::Widget::Dialog
{
	class ImageExporter : public BaseManualWidget<QDialog>
	{
		VTX_WIDGET

	  public:
		static void openDialog();

		void localize() override;
		void cancelAction();
		void saveAction();

	  protected:
		ImageExporter( QWidget * p_parent );

		void showEvent( QShowEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refresh();

	  private:
		inline static ImageExporter * _instance = nullptr;
		static ImageExporter &		  _getInstance();

		QComboBox *							   _resolutionWidget		= nullptr;
		CustomWidget::FloatFieldSliderWidget * _backgroundOpacitySlider = nullptr;
		CustomWidget::FloatFieldSliderWidget * _qualitySlider			= nullptr;

		QDialogButtonBox * _dialogButtons = nullptr;

		void _addWidget( const QString & p_label, QWidget * const p_setting, QGridLayout * const p_layout );
		void _addWidget( QWidget * const p_setting, QGridLayout * const p_layout );
	};
} // namespace VTX::UI::Widget::Dialog

#endif
