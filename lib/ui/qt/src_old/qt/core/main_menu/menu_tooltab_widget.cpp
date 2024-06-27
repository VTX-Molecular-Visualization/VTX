#include "qt/core/main_menu/menu_tooltab_widget.hpp"
#include "qt/core/main_menu/menu_toolblock_widget.hpp"
#include "qt/style.hpp"
#include "qt/widget_factory.hpp"
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::QT::Core::MainMenu
{
	MenuTooltabWidget::MenuTooltabWidget( QWidget * p_parent ) :
		VTX::UI::Core::MainMenu::MainMenuTooltab(), BaseManualWidget( p_parent )
	{
	}
	MenuTooltabWidget::~MenuTooltabWidget() {}

	void MenuTooltabWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 2, 2, 2, 2 );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Expanding );

		_horizontalLayout = new QHBoxLayout( this );
		_horizontalLayout->setContentsMargins( 0, 0, 0, 0 );
		_horizontalLayout->setSpacing( 2 );

		_horizontalLayout->addStretch( 1000 );
	}

	VTX::UI::Core::MainMenu::MainMenuToolBlock * MenuTooltabWidget::_instantiateBlock()
	{
		MenuToolBlockWidget * const newToolBlock
			= WidgetFactory::get().instantiateWidget<MenuToolBlockWidget>( this, "" );

		_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, newToolBlock );

		if ( _getToolblockCount() > 0 )
		{
			MenuToolBlockWidget * const lastToolblock
				= dynamic_cast<MenuToolBlockWidget *>( *( _getToolblocks().crbegin() ) );
			setTabOrder( lastToolblock, newToolBlock );
		}

		return newToolBlock;
	}

	void MenuTooltabWidget::_addSeparator()
	{
		QLabel * separator = new QLabel( this );
		separator->setPixmap( Style::IconConst::get().TOOLTAB_SEPARATOR );

		_horizontalLayout->insertSpacing( _horizontalLayout->count() - 1, 3 );
		_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, separator );
		_horizontalLayout->insertSpacing( _horizontalLayout->count() - 1, 3 );
	}
} // namespace VTX::UI::QT::Core::MainMenu
