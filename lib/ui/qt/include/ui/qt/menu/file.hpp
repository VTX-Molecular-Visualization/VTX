#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public Widget::BaseWidget<File, QMenu>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget( "File", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::System::New>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::System::Download>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::System::Open>() );
			addAction( MAIN_WINDOW().getAction<Action::System::OpenRecent>() );
			addAction( MAIN_WINDOW().getAction<Action::System::Save>() );
			addAction( MAIN_WINDOW().getAction<Action::System::SaveAs>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::System::Import>() );
			addAction( MAIN_WINDOW().getAction<Action::System::Export>() );
			addSeparator();
			addAction( MAIN_WINDOW().getAction<Action::System::Quit>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
