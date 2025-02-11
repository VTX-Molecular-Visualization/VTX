#ifndef __VTX_UI_QT_DIALOG_ABOUT__
#define __VTX_UI_QT_DIALOG_ABOUT__

#include "ui/qt/core/base_dialog.hpp"

namespace VTX::UI::QT::Dialog
{

	class About : public Core::BaseDialog<About>
	{
	  public:
		About()
		{
			setWindowTitle( "About" );
			setFixedSize( 400, 200 );
			setWindowFlags( Qt::Dialog );
		}

	  private:
	};

} // namespace VTX::UI::QT::Dialog

#endif
