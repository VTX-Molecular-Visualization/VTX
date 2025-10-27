#ifndef __VTX_UI_QT_TOOL_BAR_FILE__
#define __VTX_UI_QT_TOOL_BAR_FILE__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QToolBar>
namespace VTX::UI::QT::ToolBar
{

	class File : public Widget::BaseWidget<File, QToolBar>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget( "File", p_parent )
		{
			addAction( MAIN_WINDOW().getAction<Action::System::New>() );
			addAction( MAIN_WINDOW().getAction<Action::System::Download>() );
			addAction( MAIN_WINDOW().getAction<Action::System::Open>() );
			addAction( MAIN_WINDOW().getAction<Action::System::Save>() );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
