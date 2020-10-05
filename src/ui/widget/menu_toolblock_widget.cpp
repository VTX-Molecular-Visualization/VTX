#include "menu_toolblock_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void MenuToolBlockWidget::addButton( MenuToolButtonWidget & p_toolButton ) { _gridLayout->addWidget( &p_toolButton ); }
			void MenuToolBlockWidget::addButton( MenuToolButtonWidget & p_toolButton,
												 const int				p_row,
												 const int				p_column,
												 const int				p_rowspan,
												 const int				p_colspan,
												 Qt::Alignment			p_alignement )
			{
				_gridLayout->addWidget( &p_toolButton, p_row, p_column, p_rowspan, p_colspan, p_alignement );
				_gridLayout->setRowStretch( p_row, 10 );
			}

			void MenuToolBlockWidget::setTitle( const QString & p_title ) { _title->setText( p_title ); }
			void MenuToolBlockWidget::validate()
			{
				int titleRow = _gridLayout->rowCount();
				_gridLayout->addWidget( _title, titleRow, 0, 1, _gridLayout->columnCount() );
				_gridLayout->setRowStretch( titleRow, 1 );
				_gridLayout->setRowMinimumHeight( titleRow, 0 );
			}

			void MenuToolBlockWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );
				setContentsMargins( 2, 2, 2, 2 );

				QSizePolicy sizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
				setSizePolicy( sizePolicy );

				_gridLayout = new QGridLayout( this );
				_gridLayout->setObjectName( "gridLayout" );
				_gridLayout->setVerticalSpacing( 4 );
				_gridLayout->setContentsMargins( 0, 0, 0, 0 );

				_title = new QLabel( this );
				_title->setObjectName( "blockTitle" );
				_title->setAlignment( Qt::AlignCenter );
			}
			void MenuToolBlockWidget::_setupSlots() {}
			void MenuToolBlockWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
