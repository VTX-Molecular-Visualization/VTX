#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include "qt/base_widget.hpp"
#include "qt/dialog/download.hpp"
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

			// Connect to dialog.
			auto * downloadAction = addAction<Action::System::Download>();
			connect(
				downloadAction,
				&QAction::triggered,
				this,
				[ & ]()
				{
					Dialog::Download dialog;
					dialog.exec();
				}
			);

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
