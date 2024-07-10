#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "qt/base_widget.hpp"
#include "qt/helper.hpp"
#include "qt/resources.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public BaseWidget<Snapshot, QToolBar>
	{
	  public:
		Snapshot( QWidget * p_parent ) : BaseWidget<Snapshot, QToolBar>( "Snapshot", p_parent )
		{
			/*
			using namespace Helper;

			addQAction( this, { "Snapshot", "Save current image", Resources::SPRITE_SNAPSHOT.toStdString() } );
			addQAction(
				this, { "Export image", "Open dialog to export image", Resources::SPRITE_SNAPSHOT.toStdString() }
			);
			*/
		}

		virtual ~Snapshot() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
