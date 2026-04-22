#ifndef __VTX_UI_QT_TOOL_BAR_FILE__
#define __VTX_UI_QT_TOOL_BAR_FILE__

#include "ui/qt/widget/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class File : public Widget::BaseWidget<File, QToolBar>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "File" );

			addAction( Action::System::NEW );
			addAction( Action::System::DOWNLOAD );
			addAction( Action::System::OPEN );
			addAction( Action::System::SAVE );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
