#include "menu_toolblock_widget.hpp"
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
			const int			  nbRows	  = _columnsData[ iCol ].getRowCount();
			const int			  gridRowSpan = GRID_LAYOUT_ROW_COUNT / _columnsData[ iCol ].getNbRowsDisplayed();
			const Qt::Orientation orientation = nbRows <= 1 ? Qt::Orientation::Vertical : Qt::Orientation::Horizontal;

			for ( int iRow = 0; iRow < nbRows; iRow++ )
			{
				int							 gridRow	= p_startRow + iRow * gridRowSpan;
				MenuToolButtonWidget * const toolButton = _columnsData[ iCol ].getButton( iRow );
				toolButton->setOrientation( orientation );
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

	///// MenuToolBlockWidget /////
	void MenuToolBlockWidget::pushButton( MenuToolButtonWidget & p_toolButton, const int p_column )
	{
		_tmpStructure->pushWidgetInColumn( p_column, &p_toolButton );
	}
	int MenuToolBlockWidget::pushButtonInNextColumn( MenuToolButtonWidget & p_toolButton )
	{
		const int column = int( _tmpStructure->getNbColumns() );
		pushButton( p_toolButton, column );
		return column;
	}

	void MenuToolBlockWidget::setTitle( const QString & p_title ) { _title->setText( p_title ); }
	void MenuToolBlockWidget::validate()
	{
		_tmpStructure->fillGridLayout( *_gridLayout, 0 );
		delete _tmpStructure;
		_tmpStructure = nullptr;
	}

	void MenuToolBlockWidget::reset()
	{
		_gridLayout->removeWidget( _title );

		while ( _gridLayout->count() > 0 )
		{
			QLayoutItem * const item = _gridLayout->takeAt( _gridLayout->count() - 1 );

			_gridLayout->removeItem( item );

			if ( item->widget() != nullptr )
				delete item->widget();
		}
		delete _gridLayout;

		_gridLayout = new QGridLayout( this );
		_gridLayout->setObjectName( "gridLayout" );
		_gridLayout->setVerticalSpacing( 2 );
		_gridLayout->setHorizontalSpacing( 2 );
		_gridLayout->setContentsMargins( 0, 0, 0, 0 );
		_gridLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );

		if ( _tmpStructure != nullptr )
			delete _tmpStructure;

		_tmpStructure = new TmpGridStructure();
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

		_verticalLayout->addLayout( _gridLayout, 5 );
		_verticalLayout->addWidget( _title, 0, Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignBottom );
	}
	void MenuToolBlockWidget::_setupSlots() {}
	void MenuToolBlockWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu
