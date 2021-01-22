#include "menu_toolblock_widget.hpp"
#include <QBoxLayout>
#include <QSizePolicy>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				///// MenuToolBlockWidget::TmpGridStructure /////
				MenuToolBlockWidget::TmpGridStructure::~TmpGridStructure()
				{
					while ( !_columnsData.empty() )
					{
						std::vector<MenuToolButtonWidget *> * lastRow = _columnsData.back();
						_columnsData.pop_back();
						delete lastRow;
					}
				}
				void MenuToolBlockWidget::TmpGridStructure::fillGridLayout( QGridLayout & p_gridLayout, const int p_startRow )
				{
					for ( int iCol = 0; iCol < _columnsData.size(); iCol++ )
					{
						int				nbRows		= (int)_columnsData[ iCol ]->size();
						int				gridRowSpan = GRID_LAYOUT_ROW_COUNT / nbRows;
						Qt::Orientation orientation = nbRows <= 1 ? Qt::Orientation::Vertical : Qt::Orientation::Horizontal;

						for ( int iRow = 0; iRow < nbRows; iRow++ )
						{
							int							 gridRow	= p_startRow + iRow * gridRowSpan;
							MenuToolButtonWidget * const toolButton = ( *( _columnsData[ iCol ] ) )[ iRow ];
							toolButton->setOrientation( orientation );

							p_gridLayout.addWidget( toolButton, gridRow, iCol, gridRowSpan, 1 );
						}
					}
				}
				void MenuToolBlockWidget::TmpGridStructure::pushWidgetInColumn( const int p_column, MenuToolButtonWidget * const p_widget )
				{
					if ( getNbColumns() <= p_column )
						addNewColumn( p_column - ( getNbColumns() - 1 ) );

					_columnsData[ p_column ]->push_back( p_widget );
				};
				void MenuToolBlockWidget::TmpGridStructure::addNewColumn( const size_t p_nbColumns )
				{
					for ( int i = 0; i < p_nbColumns; i++ )
					{
						std::vector<MenuToolButtonWidget *> * newRows = new std::vector<MenuToolButtonWidget *>();
						newRows->reserve( MAX_ROW_COUNT );

						_columnsData.push_back( newRows );
					}
				}

				///// MenuToolBlockWidget /////
				void MenuToolBlockWidget::pushButton( MenuToolButtonWidget & p_toolButton, const int p_column ) { _tmpStructure->pushWidgetInColumn( p_column, &p_toolButton ); }
				void MenuToolBlockWidget::pushButtonInNextColumn( MenuToolButtonWidget & p_toolButton )
				{
					int column = int( _tmpStructure->getNbColumns() );
					pushButton( p_toolButton, column );
				}

				void MenuToolBlockWidget::setTitle( const QString & p_title ) { _title->setText( p_title ); }
				void MenuToolBlockWidget::validate()
				{
					_gridLayout->setRowStretch( 0, 1000 );
					_tmpStructure->fillGridLayout( *_gridLayout, 1 );
					delete _tmpStructure;
					_tmpStructure = nullptr;

					_gridLayout->setRowStretch( _gridLayout->rowCount(), 1000 );
					int titleRow = _gridLayout->rowCount();

					QVBoxLayout * const titleLayout = new QVBoxLayout();
					titleLayout->addStretch( 1000 );
					titleLayout->addWidget( _title );
					_gridLayout->addItem( titleLayout, titleRow, 0, 1, _gridLayout->columnCount() );

					_gridLayout->setRowMinimumHeight( titleRow, 16 );
				}

				void MenuToolBlockWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 2, 2, 2, 2 );

					setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

					_gridLayout = new QGridLayout( this );
					_gridLayout->setObjectName( "gridLayout" );
					_gridLayout->setVerticalSpacing( 1 );
					_gridLayout->setHorizontalSpacing( 0 );
					_gridLayout->setContentsMargins( 0, 0, 0, 0 );

					_title = new QLabel( this );
					_title->setObjectName( "blockTitle" );
					_title->setAlignment( Qt::AlignCenter );
					_title->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
				}
				void MenuToolBlockWidget::_setupSlots() {}
				void MenuToolBlockWidget::localize() {}
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
