#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public Widget::BaseWidget<File, QMenu>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setTitle( "File" );

			addAction( Action::System::NEW );
			addSeparator();
			addAction( Action::System::DOWNLOAD );
			addSeparator();
			addAction( Action::System::OPEN );
			addAction( Action::System::OPEN_RECENT )->setEnabled( false );
			addAction( Action::System::SAVE )->setEnabled( false );
			addAction( Action::System::SAVE_AS )->setEnabled( false );
			addSeparator();
			addAction( Action::System::IMPORT )->setEnabled( false );
			addAction( Action::System::EXPORT )->setEnabled( false );
			addSeparator();
			addAction( Action::System::QUIT );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
