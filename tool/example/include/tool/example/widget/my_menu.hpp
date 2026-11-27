#pragma once

#include "tool/example/actions.hpp"
#include <QMenu>
#include <ui/qt/action_registry.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/widget/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyMenu : public UI::QT::Widget::BaseWidget<MyMenu, QMenu>
	{
	  public:
		MyMenu( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "My Menu" );

			UI::QT::UI_ACTIONS().addTo( *this, Action::MY_ACTION_1 );
			addSeparator();
			UI::QT::UI_ACTIONS().addTo( *this, Action::MY_ACTION_2 );
			UI::QT::UI_ACTIONS().addTo( *this, Action::MY_ACTION_3 );
			addSeparator();
			UI::QT::UI_ACTIONS().addTo( *this, Action::MY_ACTION_4 );
		}

		~MyMenu() override = default;

	  private:
	};

} // namespace VTX::Tool::Example::Widget
