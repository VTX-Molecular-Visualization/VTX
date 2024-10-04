#ifndef __VTX_UI_QT_DIALOG_EXPORT_IMAGE__
#define __VTX_UI_QT_DIALOG_EXPORT_IMAGE__

#include "ui/qt/base_widget.hpp"
#include <QComboBox>
#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QPointer>
#include <QSlider>
#include <QSpinBox>

namespace VTX::UI::QT::Dialog
{

	class ExportImage : public BaseWidget<ExportImage, QDialog>
	{
	  public:
		ExportImage();

	  private:
		inline static const std::string _SETTING_KEY_FORMAT = "dialogExportImageFormat";

		struct _Resolution
		{
			std::string_view name;
			size_t			 width;
			size_t			 height;
		};

		// TODO: use openGL real max texture size.
		inline static const size_t _MAX_TEXTURE_SIZE = 16384;
		inline static const float  _RATIO_MIN		 = 0.3f;
		inline static const float  _RATIO_MAX		 = 3.0f;

		inline static std::vector<_Resolution> _RESOLUTIONS
			= { { "Actual size", 0, 0 },   { "SVGA", 800, 600 },	 { "XGA", 1024, 768 },
				{ "HD", 1280, 720 },	   { "WXGA", 1280, 800 },	 { "SXGA", 1280, 1024 },
				{ "WXGA+", 1440, 900 },	   { "HD+", 1600, 900 },	 { "WSXGA+", 1680, 1050 },
				{ "Full HD", 1920, 1080 }, { "WUXGA", 1920, 1200 },	 { "QHD (1440p)", 2560, 1440 },
				{ "WQXGA", 2560, 1600 },   { "4K UHD", 3840, 2160 }, { "8K UHD", 7680, 4320 },
				{ "16K UHD", 15360, 8640 } };

		QPointer<QComboBox> _comboBoxResolution;
		QPointer<QSpinBox>	_spinBoxWidth;
		QPointer<QSpinBox>	_spinBoxHeight;
		QPointer<QLabel>	_labelRatio;
		QPointer<QSlider>	_sliderRatio;

		QPointer<QLabel> _preview;

		void _onResolution();
		void _onSize();
		void _onRatio();

		void _updatePreview();
	};

} // namespace VTX::UI::QT::Dialog

#endif
