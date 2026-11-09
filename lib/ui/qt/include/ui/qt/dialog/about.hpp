#ifndef VTX_UI_QT_DIALOG_ABOUT_HPP
#define VTX_UI_QT_DIALOG_ABOUT_HPP

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

#endif
