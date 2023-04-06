#include "menu_toolblock_widget.hpp"
#include "menu_tooltab_widget.hpp"
#include <QBoxLayout>
#include <QSizePolicy>

namespace VTX::UI::Widget::MainMenu
{
	///// MenuToolBlockWidget::TmpGridStructure /////
	MenuToolBlockWidget::TmpGridStructure::~TmpGridStructure() {}
	void MenuToolBlockWidget::TmpGridStructure::fillGridLayout( QGridLayout & p_gridLayout, const int p_startRow )
	{
		for ( int iCol = 0; iCol < _columnsData.size(); iCol++ )
		{
			const int nbRows	  = _columnsData[ iCol ].getRowCount();
			const int gridRowSpan = GRID_LAYOUT_ROW_COUNT / _columnsData[ iCol ].getNbRowsDisplayed();

			for ( int iRow = 0; iRow < nbRows; iRow++ )
			{
				int							 gridRow	= p_startRow + iRow * gridRowSpan;
				MenuToolButtonWidget * const toolButton = _columnsData[ iCol ].getButton( iRow );
				const int					 iconSize = 16 + ( ( 3 - _columnsData[ iCol ].getNbRowsDisplayed() ) * 8 );
				toolButton->setIconSize( QSize( iconSize, iconSize ) );
				p_gridLayout.addWidget( toolButton, gridRow, iCol, gridRowSpan, 1 );
			}

			p_gridLayout.setColumnMinimumWidth( iCol, 64 );
		}
	}
	void MenuToolBlockWidget::TmpGridStructure::pushWidgetInColumn( const int					 p_column,
																	MenuToolButtonWidget * const p_widget )
	{
		if ( getNbColumns() <= p_column )
			addNewColumn( p_column - ( getNbColumns() - 1 ) );

		_columnsData[ p_column ].addButton( p_widget );
	};
	void MenuToolBlockWidget::TmpGridStructure::addNewColumn( const size_t p_nbColumns )
	{
		for ( int i = 0; i < p_nbColumns; i++ )
		{
			ColumnData newColumn = ColumnData( MAX_ROW_COUNT );
			_columnsData.emplace_back( newColumn );
		}
	}

	void MenuToolBlockWidget::TmpGridStructure::clear()
	{
		_columnsData.clear();
		_columnsData.reserve( MAX_COLUMN_COUNT );
	}

	///// MenuToolBlockWidget /////
	void MenuToolBlockWidget::pushButton( MenuToolButtonWidget & p_toolButton, const int p_column )
	{
		_tmpStructure.pushWidgetInColumn( p_column, &p_toolButton );
		_buttons.emplace_back( &p_toolButton );

		connect( &p_toolButton.arrowNavigation,
				 &Navigation::ArrowNavigation::navigateToLeft,
				 this,
				 &MenuToolBlockWidget::_navigationToLeft );

		connect( &p_toolButton.arrowNavigation,
				 &Navigation::ArrowNavigation::navigateToRight,
				 this,
				 &MenuToolBlockWidget::_navigationToRight );

		connect( &p_toolButton.arrowNavigation,
				 &Navigation::ArrowNavigation::navigateToUp,
				 this,
				 &MenuToolBlockWidget::_navigationToUp );

		connect( &p_toolButton.arrowNavigation,
				 &Navigation::ArrowNavigation::navigateToDown,
				 this,
				 &MenuToolBlockWidget::_navigationToDown );
	}

	void MenuToolBlockWidget::_navigationToLeft() const
	{
		QWidget * const focusedWidget	 = focusWidget();
		const int		focusedItemIndex = _gridLayout->indexOf( focusedWidget );
		int				focusedItemRow;
		int				focusedItemColumn;
		int				focusedItemRowSpan;
		int				focusedItemColumnSpan;
		_gridLayout->getItemPosition(
			focusedItemIndex, &focusedItemRow, &focusedItemColumn, &focusedItemRowSpan, &focusedItemColumnSpan );

		focusedItemColumn -= focusedItemColumnSpan;
		focusedItemRow += focusedItemRow == 0 ? 0 : ( focusedItemRowSpan - 1 ) / 2;

		if ( focusedItemColumn >= 0 )
		{
			QLayoutItem * const newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );
			newFocusedItem->widget()->setFocus( Qt::FocusReason::BacktabFocusReason );
		}
		else
		{
			const MenuToolBlockWidget * previousToolBlock
				= getTooltab() == nullptr ? this : getTooltab()->getPreviousToolBlock( this );

			QLayoutItem * previousItem = previousToolBlock->_gridLayout->itemAtPosition(
				focusedItemRow, previousToolBlock->_gridLayout->columnCount() - 1 );

			const bool			   nextItemIsValid = previousItem != nullptr && previousItem->widget()->isEnabled();
			MenuToolButtonWidget * newFocusedWidget;

			if ( nextItemIsValid )
			{
				newFocusedWidget = dynamic_cast<MenuToolButtonWidget *>( previousItem->widget() );
			}
			else
			{
				if ( previousItem == nullptr )
					previousItem
						= previousToolBlock->_gridLayout->itemAt( previousToolBlock->_gridLayout->count() - 1 );

				newFocusedWidget = _findPreviousButton( previousToolBlock, previousItem, focusedItemRow );
			}

			if ( newFocusedWidget != nullptr )
				newFocusedWidget->setFocus( Qt::FocusReason::BacktabFocusReason );
		}
	}
	void MenuToolBlockWidget::_navigationToRight() const
	{
		QWidget * const focusedWidget	 = focusWidget();
		const int		focusedItemIndex = _gridLayout->indexOf( focusedWidget );
		int				focusedItemRow;
		int				focusedItemColumn;
		int				focusedItemRowSpan;
		int				focusedItemColumnSpan;
		_gridLayout->getItemPosition(
			focusedItemIndex, &focusedItemRow, &focusedItemColumn, &focusedItemRowSpan, &focusedItemColumnSpan );

		focusedItemColumn += focusedItemColumnSpan;
		focusedItemRow += focusedItemRow == 0 ? 0 : ( focusedItemRowSpan - 1 ) / 2;

		if ( focusedItemColumn < _gridLayout->columnCount() )
		{
			QLayoutItem * const newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );
			newFocusedItem->widget()->setFocus( Qt::FocusReason::TabFocusReason );
		}
		else
		{
			const MenuToolBlockWidget * nextToolBlock
				= getTooltab() == nullptr ? this : getTooltab()->getNextToolBlock( this );

			QLayoutItem * nextItem		  = nextToolBlock->_gridLayout->itemAtPosition( focusedItemRow, 0 );
			const bool	  nextItemIsValid = nextItem != nullptr && nextItem->widget()->isEnabled();

			MenuToolButtonWidget * newFocusedWidget;

			if ( nextItemIsValid )
			{
				newFocusedWidget = dynamic_cast<MenuToolButtonWidget *>( nextItem->widget() );
			}
			else
			{
				if ( nextItem == nullptr )
					nextItem = nextToolBlock->_gridLayout->itemAt( 0 );

				newFocusedWidget = _findNextButton( nextToolBlock, nextItem, focusedItemRow );
			}

			if ( newFocusedWidget != nullptr )
				newFocusedWidget->setFocus( Qt::FocusReason::TabFocusReason );
		}
	}

	void MenuToolBlockWidget::_navigationToUp() const
	{
		QWidget * const focusedWidget	 = focusWidget();
		const int		focusedItemIndex = _gridLayout->indexOf( focusedWidget );
		int				focusedItemRow;
		int				focusedItemColumn;
		int				focusedItemRowSpan;
		int				focusedItemColumnSpan;
		_gridLayout->getItemPosition(
			focusedItemIndex, &focusedItemRow, &focusedItemColumn, &focusedItemRowSpan, &focusedItemColumnSpan );

		focusedItemRow -= focusedItemRowSpan;

		if ( focusedItemRow < 0 )
			focusedItemRow = _gridLayout->rowCount() - 1;

		QLayoutItem * newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );
		while ( newFocusedItem == nullptr || !newFocusedItem->widget()->isEnabled() )
		{
			focusedItemRow -= 1;
			newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );
		}
		newFocusedItem->widget()->setFocus( Qt::FocusReason::BacktabFocusReason );
	}

	void MenuToolBlockWidget::_navigationToDown() const
	{
		QWidget * const focusedWidget	 = focusWidget();
		const int		focusedItemIndex = _gridLayout->indexOf( focusedWidget );
		int				focusedItemRow;
		int				focusedItemColumn;
		int				focusedItemRowSpan;
		int				focusedItemColumnSpan;
		_gridLayout->getItemPosition(
			focusedItemIndex, &focusedItemRow, &focusedItemColumn, &focusedItemRowSpan, &focusedItemColumnSpan );

		focusedItemRow += focusedItemRowSpan;

		if ( focusedItemRow >= _gridLayout->rowCount() )
			focusedItemRow = 0;

		QLayoutItem * newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );

		if ( newFocusedItem == nullptr )
		{
			focusedItemRow = 0;
			while ( focusedItemRow < _gridLayout->rowCount() && newFocusedItem == nullptr
					|| !newFocusedItem->widget()->isEnabled() )
			{
				newFocusedItem = _gridLayout->itemAtPosition( focusedItemRow, focusedItemColumn );
				focusedItemRow++;
			}
		}

		newFocusedItem->widget()->setFocus( Qt::FocusReason::TabFocusReason );
	}

	MenuToolButtonWidget * MenuToolBlockWidget::_findPreviousButton( const MenuToolBlockWidget * const p_toolBlock,
																	 QLayoutItem * const			   p_item,
																	 const int						   p_row ) const
	{
		const MenuToolBlockWidget * toolBlockSeeker		= p_toolBlock;
		bool						previousItemIsValid = false;

		QLayoutItem * previousItem		= p_item;
		int			  previousItemIndex = toolBlockSeeker->_gridLayout->indexOf( previousItem ) - 1;

		do
		{
			while ( previousItemIndex >= 0 )
			{
				previousItem = toolBlockSeeker->_gridLayout->itemAt( previousItemIndex );

				previousItemIsValid = ( previousItem->widget()->focusPolicy() & Qt::FocusPolicy::TabFocus ) > 0
									  && previousItem->widget()->isEnabled();

				if ( previousItemIsValid )
					break;

				previousItemIndex--;
			}

			if ( !previousItemIsValid )
			{
				toolBlockSeeker = getTooltab()->getPreviousToolBlock( toolBlockSeeker );
				previousItem	= toolBlockSeeker->_gridLayout->itemAtPosition(
					   p_row, toolBlockSeeker->_gridLayout->columnCount() - 1 );
				previousItemIndex = toolBlockSeeker->_gridLayout->indexOf( previousItem );
			}

		} while ( toolBlockSeeker != this && !previousItemIsValid );

		return previousItemIsValid ? dynamic_cast<MenuToolButtonWidget *>( previousItem->widget() ) : nullptr;
	}

	MenuToolButtonWidget * MenuToolBlockWidget::_findNextButton( const MenuToolBlockWidget * const p_toolBlock,
																 QLayoutItem * const			   p_item,
																 const int						   p_row ) const
	{
		const MenuToolBlockWidget * toolBlockSeeker = p_toolBlock;
		bool						nextItemIsValid = false;

		QLayoutItem * nextItem		= p_item;
		int			  nextItemIndex = toolBlockSeeker->_gridLayout->indexOf( nextItem ) + 1;

		do
		{
			while ( nextItemIndex < toolBlockSeeker->_gridLayout->count() )
			{
				nextItem = toolBlockSeeker->_gridLayout->itemAt( nextItemIndex );

				nextItemIsValid = ( nextItem->widget()->focusPolicy() & Qt::FocusPolicy::TabFocus ) > 0
								  && nextItem->widget()->isEnabled();

				if ( nextItemIsValid )
					break;

				nextItemIndex++;
			}

			toolBlockSeeker = getTooltab()->getNextToolBlock( toolBlockSeeker );
			nextItemIndex	= 0;
		} while ( toolBlockSeeker != this && !nextItemIsValid );

		return nextItemIsValid ? dynamic_cast<MenuToolButtonWidget *>( nextItem->widget() ) : nullptr;
	}

	int MenuToolBlockWidget::pushButtonInNextColumn( MenuToolButtonWidget & p_toolButton )
	{
		const int column = int( _tmpStructure.getNbColumns() );
		pushButton( p_toolButton, column );
		return column;
	}

	void MenuToolBlockWidget::setTitle( const QString & p_title ) { _title->setText( p_title ); }
	void MenuToolBlockWidget::validate()
	{
		_tmpStructure.fillGridLayout( *_gridLayout, 0 );
		_tmpStructure.clear();
	}

	void MenuToolBlockWidget::reset()
	{
		for ( int i = 0; i < _gridLayout->columnCount(); i++ )
			_gridLayout->setColumnMinimumWidth( i, 0 );

		while ( _gridLayout->count() > 0 )
		{
			QLayoutItem * const item = _gridLayout->takeAt( _gridLayout->count() - 1 );

			_gridLayout->removeItem( item );

			if ( item->widget() != nullptr )
				delete item->widget();
		}

		_tmpStructure.clear();
	}

	void MenuToolBlockWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 2, 2, 2, 2 );

		setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

		_verticalLayout = new QVBoxLayout( this );
		_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

		_gridLayout = new QGridLayout();
		_gridLayout->setObjectName( "gridLayout" );
		_gridLayout->setVerticalSpacing( 2 );
		_gridLayout->setHorizontalSpacing( 2 );
		_gridLayout->setContentsMargins( 0, 0, 0, 0 );
		_gridLayout->setSizeConstraint( QLayout::SizeConstraint::SetDefaultConstraint );

		_title = new QLabel( this );
		_title->setObjectName( "blockTitle" );
		_title->setFocusPolicy( Qt::FocusPolicy::NoFocus );

		_verticalLayout->addLayout( _gridLayout, 5 );
		_verticalLayout->addWidget( _title, 0, Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignBottom );
	}
	void MenuToolBlockWidget::_setupSlots() {}
	void MenuToolBlockWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu
