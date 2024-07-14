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
			using namespace UI::Action;

			addAction<System::New>();
			addSeparator();

			// Connect to dialog.
			auto * aDownload = addAction<System::Download>();
			connect(
				aDownload,
				&QAction::triggered,
				this,
				[]()
				{
					Dialog::Download dialog;
					dialog.exec();
				}
			);

			addSeparator();
			addAction<System::Open>();
			addAction<System::OpenRecent>();
			addAction<System::Save>();
			addAction<System::SaveAs>();
			addSeparator();
			addAction<System::Import>();
			addAction<System::Export>();
			addSeparator();
			addAction<System::Quit>();
		}
		virtual ~File() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
