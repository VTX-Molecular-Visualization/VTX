#include "setting_menu_item.hpp"
#include "setting_widget.hpp"

namespace VTX::UI::Widget::Settings
{
	SettingMenuItem::SettingMenuItem( SettingWidget * const p_parent,
									  const SETTING_MENU	p_menu,
									  QWidget * const		p_content,
									  const QString &		p_title ) :
		QObject( p_parent->widget() ),
		_parent( p_parent ), _content( p_content ), _menu( p_menu ), _title( p_title )
	{
		_button = new QPushButton( p_parent );
		_button->setText( p_title );

		setVisible( false );

		connect( _button, &QPushButton::clicked, this, &SettingMenuItem::_onButtonPushed );
	}

	void SettingMenuItem::setVisible( bool p_visible )
	{
		if ( p_visible )
			_content->show();
		else
			_content->hide();
	}

	void SettingMenuItem::_onButtonPushed() { _parent->setCurrentMenu( _menu ); }
} // namespace VTX::UI::Widget::Settings
