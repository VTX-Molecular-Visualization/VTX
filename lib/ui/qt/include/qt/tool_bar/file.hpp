#ifndef __VTX_UI_QT_TOOL_BAR_FILE__
#define __VTX_UI_QT_TOOL_BAR_FILE__

#include "qt/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class File : public BaseWidget<File, QToolBar>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget<File, QToolBar>( "Camera", p_parent )
		{
			using namespace UI::Action;

			setLayout( new QGridLayout() );

			addAction<System::New>();
			addAction<System::Download>();
			addAction<System::Open>();
			addAction<System::Save>();

			// Set tool button small.
			// setToolButtonStyle( Qt::ToolButtonIconOnly );
		}

		virtual ~File() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
