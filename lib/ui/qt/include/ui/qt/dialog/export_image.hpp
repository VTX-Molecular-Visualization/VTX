#ifndef __VTX_UI_QT_DIALOG_EXPORT_IMAGE__
#define __VTX_UI_QT_DIALOG_EXPORT_IMAGE__

#include "ui/qt/base_widget.hpp"
#include <QComboBox>
#include <QDialog>
#include <QPointer>

namespace VTX::UI::QT::Dialog
{

	class ExportImage : public BaseWidget<ExportImage, QDialog>
	{
	  public:
		ExportImage();

	  private:
		inline static const std::string _SETTING_KEY_FORMAT = "dialogExportImageFormat";

		QPointer<QComboBox> _comboBoxResolution;
	};

} // namespace VTX::UI::QT::Dialog

#endif
