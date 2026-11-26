#pragma once

#include "ui/qt/dialog/base_dialog.hpp"

namespace VTX::UI::QT::Dialog
{

	class About : public BaseDialog<About>
	{
	  public:
		About()
		{
			setWindowTitle( "About" );
			setFixedSize( 400, 200 );
		}

	  private:
	};

} // namespace VTX::UI::QT::Dialog
