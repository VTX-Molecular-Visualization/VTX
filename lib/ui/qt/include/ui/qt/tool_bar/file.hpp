#ifndef __VTX_UI_QT_TOOL_BAR_FILE__
#define __VTX_UI_QT_TOOL_BAR_FILE__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
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

			UI_ACTIONS().addTo( *this, Action::System::NEW );
			UI_ACTIONS().addTo( *this, Action::System::DOWNLOAD );
			UI_ACTIONS().addTo( *this, Action::System::OPEN );
			UI_ACTIONS().addTo( *this, Action::System::SAVE );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
