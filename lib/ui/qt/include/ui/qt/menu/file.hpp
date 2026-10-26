#ifndef VTX_UI_QT_MENU_FILE_HPP
#define VTX_UI_QT_MENU_FILE_HPP

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class File : public Widget::BaseWidget<File, QMenu>
	{
	  public:
		File( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setTitle( "File" );

			UI_ACTIONS().addTo( *this, Action::System::NEW );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::System::DOWNLOAD );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::System::OPEN );
			UI_ACTIONS().addTo( *this, Action::System::OPEN_RECENT )->setEnabled( false );
			UI_ACTIONS().addTo( *this, Action::System::SAVE )->setEnabled( false );
			UI_ACTIONS().addTo( *this, Action::System::SAVE_AS )->setEnabled( false );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::System::IMPORT )->setEnabled( false );
			UI_ACTIONS().addTo( *this, Action::System::EXPORT )->setEnabled( false );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::System::QUIT );
		}

	  private:
	};

} // namespace VTX::UI::QT::Menu

#endif
