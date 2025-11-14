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

			addAction<Action::System::New>();
			addSeparator();
			addAction<Action::System::Download>();
			addSeparator();
			addAction<Action::System::Open>();
			addAction<Action::System::OpenRecent>()->setEnabled( false );
			addAction<Action::System::Save>()->setEnabled( false );
			addAction<Action::System::SaveAs>()->setEnabled( false );
			addSeparator();
			addAction<Action::System::Import>()->setEnabled( false );
			addAction<Action::System::Export>()->setEnabled( false );
			addSeparator();
			addAction<Action::System::Quit>();
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
