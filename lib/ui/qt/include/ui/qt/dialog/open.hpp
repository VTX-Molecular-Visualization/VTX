#ifndef VTX_UI_QT_DIALOG_OPEN_HPP
#define VTX_UI_QT_DIALOG_OPEN_HPP

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
