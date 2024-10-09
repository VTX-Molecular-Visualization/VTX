#ifndef __VTX_UI_QT_TOOL_BAR_FILE__
#define __VTX_UI_QT_TOOL_BAR_FILE__

#include "ui/qt/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class File : public BaseWidget<File, QToolBar>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget<File, QToolBar>( "File", p_parent )
		{
			addAction<Action::System::New>();
			addAction<Action::System::Download>();
			addAction<Action::System::Open>();
			addAction<Action::System::Save>();
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
