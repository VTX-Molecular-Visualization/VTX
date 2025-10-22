#ifndef __VTX_UI_QT_DIALOG_OPEN__
#define __VTX_UI_QT_DIALOG_OPEN__

#include "ui/qt/dialog/base_dialog.hpp"
#include <QFileDialog>

namespace VTX::UI::QT::Dialog
{

	class Open : public BaseDialog<QFileDialog>
	{
	  public:
		Open();

		void save( Settings & ) override;
		void restore( const Settings & ) override;

	  private:
		inline static const std::string _SETTING_KEY_FOLDER = "dialogOpenLastOpenFolder";

		QString _lastOpenFolder;
	};

} // namespace VTX::UI::QT::Dialog

#endif
