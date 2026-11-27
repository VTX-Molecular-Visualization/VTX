#pragma once

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
