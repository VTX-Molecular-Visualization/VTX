#ifndef __VTX_UI_QT_DIALOG_EXPORT_IMAGE__
#define __VTX_UI_QT_DIALOG_EXPORT_IMAGE__

#include "ui/qt/dialog/base_dialog.hpp"
#include "ui/qt/settings.hpp"
#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>

namespace VTX::UI::QT::Dialog
{

	class ExportImage : public BaseDialog<ExportImage>
	{
	  public:
		ExportImage();

	  private:
		// Widgets.
		QPointer<QComboBox> _comboBoxResolution;
		QPointer<QSpinBox>	_spinBoxWidth;
		QPointer<QSpinBox>	_spinBoxHeight;
		QPointer<QLabel>	_labelRatioValue;
		QPointer<QSlider>	_sliderRatio;
		QPointer<QComboBox> _comboBoxFormat;
		QPointer<QLabel>	_labelBackgroundOpacity;
		QPointer<QSlider>	_sliderBackgroundOpacity;
		QPointer<QLabel>	_labelBackgroundOpacityValue;
		QPointer<QLabel>	_preview;
		QPointer<QWidget>	_previewContainer;
		QPointer<QTimer>	_previewTimer;

		// Logic.
		QString _lastExportFolder;

		// Events.
		void _onResolution( const int );
		void _onSize();
		void _onRatio();
		void _onFormat( const int );
		void _onBackgroundOpacity();
		void _schedulePreviewUpdate();
		void _updatePreviewSize();
		void _updatePreview();
	};

} // namespace VTX::UI::QT::Dialog

#endif
