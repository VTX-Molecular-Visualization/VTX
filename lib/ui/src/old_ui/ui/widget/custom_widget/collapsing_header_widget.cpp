#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/vtx_app.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void CollapsingHeaderWidget::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setContentsMargins( 0, 0, 0, 0 );
		_mainLayout = new QVBoxLayout( this );
		_mainLayout->setContentsMargins( 0, 0, 0, 0 );
		_mainLayout->setSpacing( 0 );

		_headerWidget = new QPushButton( this );
		_headerWidget->setObjectName( p_name + "_header" );
		_headerWidget->setContentsMargins( 0, 0, 0, 0 );
		_headerWidget->setFlat( true );

		_headerLayout = new QHBoxLayout( _headerWidget );
		_headerLayout->setContentsMargins( 0, 0, 0, 0 );
		_headerLayout->setAlignment( Qt::AlignCenter );

		_iconLabel = new QLabel( _headerWidget );
		_iconLabel->setObjectName( p_name + "_header_icon" );

		_submenuButton = new QPushButton( _headerWidget );
		_submenuButton->setObjectName( p_name + "_submenu_icon" );
		_submenuButton->setFlat( true );
		_submenuButton->setIcon( QPixmap( ":/sprite/menu_icon.png" ) );
		_submenuButton->setVisible( false );

		_submenu = new QMenu( _submenuButton );
		_submenu->hide();

		_titleLabel = new QLabel();
		QFont font	= QFont( _titleLabel->font() );
		font.setBold( true );
		_titleLabel->setFont( font );
		_titleLabel->setObjectName( p_name + "_header_title" );

		_headerLayout->addStretch( 100 );
		_headerLayout->addWidget( _iconLabel );
		_headerLayout->addWidget( _titleLabel );
		_headerLayout->addStretch( 100 );
		_headerLayout->addWidget( _submenuButton );

		_mainLayout->addWidget( _headerWidget );
		_mainLayout->addStretch( 100 );
	}

	void CollapsingHeaderWidget::_setupSlots()
	{
		connect( _headerWidget, &QPushButton::clicked, this, &CollapsingHeaderWidget::toggleBodyVisibility );
		connect( _submenuButton, &QPushButton::clicked, this, &CollapsingHeaderWidget::_displaySubmenu );
	}

	void CollapsingHeaderWidget::localize() {};

	void CollapsingHeaderWidget::setMenu( QMenu * const p_menu )
	{
		if ( _submenu != nullptr )
			delete _submenu;

		_submenu = p_menu;

		_submenuButton->setVisible( _submenu != nullptr );
	}

	void CollapsingHeaderWidget::_displaySubmenu()
	{
		if ( _submenu == nullptr )
			return;

		_submenu->show();

		QPoint submenuPos = mapToGlobal( _submenuButton->pos() );
		submenuPos.setY( submenuPos.y() + _submenuButton->height() );

		if ( submenuPos.x() + _submenu->width() > VTXApp::get().getMainWindow().width() )
		{
			submenuPos.setX( submenuPos.x() - _submenu->width() + _submenuButton->width() );
		}

		if ( submenuPos.y() + _submenu->height() > VTXApp::get().getMainWindow().height() )
		{
			submenuPos.setY( submenuPos.y() - _submenu->height() - ( _submenuButton->height() * 2 ) );
		}

		_submenu->move( submenuPos );
	}

} // namespace VTX::UI::Widget::CustomWidget
