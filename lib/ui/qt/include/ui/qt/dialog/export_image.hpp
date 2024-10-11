#ifndef __VTX_UI_QT_DIALOG_EXPORT_IMAGE__
#define __VTX_UI_QT_DIALOG_EXPORT_IMAGE__

#include "ui/qt/core/base_dialog.hpp"
#include "ui/qt/settings.hpp"
#include <QComboBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPointer>
#include <QSlider>
#include <QSpinBox>
#include <util/image.hpp>

namespace VTX::UI::QT::Dialog
{

	class ExportImage : public Core::BaseDialog<ExportImage>, public Savable
	{
	  public:
		ExportImage();

		void save() override;
		void restore() override;

	  private:
		struct _Resolution
		{
			std::string_view name;
			size_t			 width;
			size_t			 height;
		};

		// Constants.
		inline static const std::string _SETTING_KEY_WIDTH	 = "dialogExportImageWidth";
		inline static const std::string _SETTING_KEY_HEIGHT	 = "dialogExportImageHeight";
		inline static const std::string _SETTING_KEY_FORMAT	 = "dialogExportImageFormat";
		inline static const std::string _SETTING_KEY_OPACITY = "dialogExportImageBackgroundOpacity";
		inline static const std::string _SETTING_KEY_FOLDER	 = "dialogExportImageLastExportFolder";

		// TODO: use openGL real max texture size.
		inline static const size_t _MAX_TEXTURE_SIZE = 16384;
		inline static const double _RATIO_MIN		 = 0.05;
		inline static const double _RATIO_MAX		 = 5.0;

		inline static std::array<_Resolution, 16> _RESOLUTIONS = { _Resolution { "Current", 0, 0 },
																   { "SVGA", 800, 600 },
																   { "XGA", 1024, 768 },
																   { "HD", 1280, 720 },
																   { "WXGA", 1280, 800 },
																   { "SXGA", 1280, 1024 },
																   { "WXGA+", 1440, 900 },
																   { "HD+", 1600, 900 },
																   { "WSXGA+", 1680, 1050 },
																   { "Full HD", 1920, 1080 },
																   { "WUXGA", 1920, 1200 },
																   { "QHD", 2560, 1440 },
																   { "WQXGA", 2560, 1600 },
																   { "4K UHD", 3840, 2160 },
																   { "8K UHD", 7680, 4320 },
																   { "16K UHD", 15360, 8640 } };

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
