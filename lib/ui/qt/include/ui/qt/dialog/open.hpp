#ifndef __VTX_UI_QT_DIALOG_OPEN__
#define __VTX_UI_QT_DIALOG_OPEN__

#include "ui/qt/dialog/base_dialog.hpp"
#include <QFileDialog>

namespace VTX::UI::QT::Dialog
{

	class Open : public BaseDialog<Open, QFileDialog>, public ISavable
	{
	  public:
		Open();

		void save() override;
		void restore() override;

	  private:
		inline static const std::string _SETTING_KEY_FOLDER = "dialogOpenLastOpenFolder";

		QString _lastOpenFolder;
	};

} // namespace VTX::UI::QT::Dialog

#endif
