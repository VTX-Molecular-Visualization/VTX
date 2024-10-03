#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include "ui/qt/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public BaseWidget<File, QMenu>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget<File, QMenu>( "File", p_parent )
		{
			addAction<Action::System::New>();
			addSeparator();
			addAction<Action::System::Download>();
			addSeparator();
			addAction<Action::System::Open>();
			addAction<Action::System::OpenRecent>();
			addAction<Action::System::Save>();
			addAction<Action::System::SaveAs>();
			addSeparator();
			addAction<Action::System::Import>();
			addAction<Action::System::Export>();
			addSeparator();
			addAction<Action::System::Quit>();
		}
		virtual ~File() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
