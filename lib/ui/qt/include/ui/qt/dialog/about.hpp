#ifndef __VTX_UI_QT_DIALOG_ABOUT__
#define __VTX_UI_QT_DIALOG_ABOUT__

#include "ui/qt/base_widget.hpp"
#include <QDialog>

namespace VTX::UI::QT::Dialog
{

	class About : public BaseWidget<About, QDialog>
	{
	  public:
		About( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "About" );
			setFixedSize( 400, 200 );
			setWindowFlags( Qt::Dialog );
		}

	  private:
	};

} // namespace VTX::UI::QT::Dialog

#endif
