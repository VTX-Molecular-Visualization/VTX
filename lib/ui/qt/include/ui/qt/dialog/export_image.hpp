#ifndef __VTX_UI_QT_DIALOG_EXPORT_IMAGE__
#define __VTX_UI_QT_DIALOG_EXPORT_IMAGE__

#include "ui/qt/dialog/base_dialog.hpp"
#include "ui/qt/settings.hpp"
#include <QComboBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPointer>
#include <QSlider>
#include <QSpinBox>
#include <array>
#include <string_view>
#include <util/image.hpp>

namespace VTX::UI::QT::Dialog
{

	class ExportImage : public BaseDialog<ExportImage>
	{
	  public:
		ExportImage();

	  private:
		// Constants.

		// TODO: use openGL real max texture size.
		inline static const size_t _MAX_TEXTURE_SIZE = 16384;
		inline static const double _RATIO_MIN		 = 0.05;
		inline static const double _RATIO_MAX		 = 5.0;

		inline static const std::array<std::string_view, 2> _FORMATS
			= { "PNG - Portable Network Graphics", "JPEG - Joint Photographic Experts Group" };

		// Widgets.
		QPointer<QComboBox>	  _comboBoxResolution;
		QPointer<QSpinBox>	  _spinBoxWidth;
		QPointer<QSpinBox>	  _spinBoxHeight;
		QPointer<QLabel>	  _labelRatioValue;
		QPointer<QSlider>	  _sliderRatio;
		QPointer<QComboBox>	  _comboBoxFormat;
		QPointer<QHBoxLayout> _layoutBackgroundOpacity;
		QPointer<QLabel>	  _labelBackgroundOpacity;
		QPointer<QSlider>	  _sliderBackgroundOpacity;
		QPointer<QLabel>	  _labelBackgroundOpacityValue;
		QPointer<QLabel>	  _preview;

		// Logic.
		QString _lastExportFolder;

		// Events.
		void _onResolution( const int );
		void _onSize();
		void _onRatio();
		void _onFormat( const int );
		void _onBackgroundOpacity();

		void _updatePreview();
	};

} // namespace VTX::UI::QT::Dialog

#endif
