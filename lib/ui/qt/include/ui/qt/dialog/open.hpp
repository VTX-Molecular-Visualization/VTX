#ifndef __VTX_UI_QT_DIALOG_OPEN__
#define __VTX_UI_QT_DIALOG_OPEN__

#include "ui/qt/dialog/base_dialog.hpp"
#include <QFileDialog>

namespace VTX::UI::QT::Dialog
{

	class Open : public BaseDialog<Open, QFileDialog>
	{
	  public:
		Open();
	};

} // namespace VTX::UI::QT::Dialog

#endif
