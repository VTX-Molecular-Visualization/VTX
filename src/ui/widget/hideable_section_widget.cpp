#include "hideable_section_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void HideableSectionWidget::_setupUi( const QString & p_name )
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

				_titleLabel = new QLabel();
				QFont font	= QFont( _titleLabel->font() );
				font.setBold( true );
				_titleLabel->setFont( font );
				_titleLabel->setObjectName( p_name + "_header_title" );

				_headerLayout->addStretch( 100 );
				_headerLayout->addWidget( _iconLabel );
				_headerLayout->addWidget( _titleLabel );
				_headerLayout->addStretch( 100 );

				_mainLayout->addWidget( _headerWidget );
				_mainLayout->addStretch( 100 );
			}

			void HideableSectionWidget::_setupSlots() { connect( _headerWidget, &QPushButton::clicked, this, &HideableSectionWidget::toggleSectionVisibility ); }

			void HideableSectionWidget::localize() {};

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
