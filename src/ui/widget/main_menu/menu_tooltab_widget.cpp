#include "menu_tooltab_widget.hpp"
#include "style.hpp"
#include <QLabel>
#include <QPixmap>

namespace VTX::UI::Widget::MainMenu
{
	MenuTooltabWidget::MenuTooltabWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
	MenuTooltabWidget::~MenuTooltabWidget() {}

	void MenuTooltabWidget::addToolBlock( MenuToolBlockWidget * const p_widget )
	{
		if ( _horizontalLayout->count() > 1 )
			_addSeparator();

		p_widget->setTooltab( this );
		_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, p_widget );

		if ( _toolBlocks.size() > 0 )
			setTabOrder( *_toolBlocks.crbegin(), p_widget );

		_toolBlocks.emplace_back( p_widget );
	}

	const MenuToolBlockWidget * const MenuTooltabWidget::getPreviousToolBlock( const MenuToolBlockWidget * const p_from,
																			   const bool p_loop ) const
	{
		const MenuToolBlockWidget * previous = p_loop ? *_toolBlocks.crbegin() : nullptr;

		for ( std::vector<MenuToolBlockWidget *>::const_iterator it = _toolBlocks.cbegin(); it != _toolBlocks.cend();
			  it++ )
		{
			if ( *it == p_from )
				return previous;

			previous = *it;
		}

		return nullptr;
	}
	const MenuToolBlockWidget * const MenuTooltabWidget::getNextToolBlock( const MenuToolBlockWidget * const p_from,
																		   const bool p_loop ) const
	{
		const MenuToolBlockWidget * next = p_loop ? *_toolBlocks.cbegin() : nullptr;

		for ( std::vector<MenuToolBlockWidget *>::const_reverse_iterator it = _toolBlocks.crbegin();
			  it != _toolBlocks.crend();
			  it++ )
		{
			if ( *it == p_from )
				return next;

			next = *it;
		}

		return nullptr;
	}

	MenuToolBlockWidget & MenuTooltabWidget::getToolBlock( const std::string & p_toolBlockName )
	{
		for ( std::vector<MenuToolBlockWidget *>::const_reverse_iterator it = _toolBlocks.crbegin();
			  it != _toolBlocks.crend();
			  it++ )
		{
			if ( ( *it )->getTitle() == p_toolBlockName )
				return *( *it );
		}

		MenuToolBlockWidget * newToolBlock
			= WidgetFactory::get().instantiateWidget<MenuToolBlockWidget>( this, "toolBlock" );
		newToolBlock->setTitle( QString::fromStdString( p_toolBlockName ) );
		addToolBlock( newToolBlock );

		return *newToolBlock;
	}

	void MenuTooltabWidget::_addSeparator()
	{
		QLabel * separator = new QLabel( this );
		separator->setPixmap( Style::IconConst::get().TOOLTAB_SEPARATOR );

		_horizontalLayout->insertSpacing( _horizontalLayout->count() - 1, 3 );
		_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, separator );
		_horizontalLayout->insertSpacing( _horizontalLayout->count() - 1, 3 );
	}

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
} // namespace VTX::UI::Widget::MainMenu
