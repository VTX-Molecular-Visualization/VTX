#ifndef __VTX_UI_QT_MENU_FILE__
#define __VTX_UI_QT_MENU_FILE__

#include "qt/base_widget.hpp"
#include "qt/helper.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public BaseWidget<File, QMenu>
	{
	  public:
		File( QWidget * p_parent ) : BaseWidget<File, QMenu>( "File", p_parent )
		{
			using namespace Helper;

			addQAction( this, { "New", "Create a new project" } );
			addSeparator();
			addQAction( this, { "Download", "Download structure from PDB id" } );
			addSeparator();
			addQAction( this, { "Open", "Open a project or a molecular file" } );
			addQAction( this, { "Open recent" } );
			addQAction( this, { "Save", "Save project" } );
			addQAction( this, { "Save as...", "Copy project in a new save" } );
			addSeparator();
			addQAction( this, { "Import" } );
			addQAction( this, { "Export" } );
			addSeparator();
			addQAction( this, { "Quit", "Exit software" } );
		}
		virtual ~File() {}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
