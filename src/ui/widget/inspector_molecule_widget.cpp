#include "inspector_molecule_widget.hpp"
#include "style.hpp"
#include <QFont>
#include <QMargins>
#include <QPixmap>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void InspectorMoleculeWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );
				setContentsMargins( 0, 0, 0, 0 );

				_verticalLayout = new QVBoxLayout( this );
				_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

				_headerWidget = new QWidget( this );
				_headerWidget->setObjectName( "inspector_item_header" );
				_headerWidget->setContentsMargins( 0, 0, 0, 0 );
				_headerLayout = new QHBoxLayout( _headerWidget );
				_headerLayout->setContentsMargins( 0, 1, 0, 1 );
				_headerLayout->setSpacing( Style::INSPECTOR_HEADER_ICON_SPACING );
				_headerLayout->setAlignment( Qt::AlignVCenter );

				_foldState = new QPushButton( _headerWidget );
				_foldState->setContentsMargins( 0, 0, 0, 0 );
				_foldState->setFlat( true );
				_foldState->setIconSize( QSize( Style::INSPECTOR_HEADER_HEIGHT / 2, Style::INSPECTOR_HEADER_HEIGHT / 2 ) );

				_symbolWidget = new QLabel( _headerWidget );
				_symbolWidget->setPixmap( QPixmap( ":/sprite/symbol/molecule_symbol_icon.png" ) );

				_enableWidget = new QCheckBox( _headerWidget );
				_enableWidget->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

				_nameWidget = new QLabel();
				QFont font	= QFont( _nameWidget->font() );
				font.setBold( true );
				_nameWidget->setFont( font );

				_headerLayout->addWidget( _foldState );
				_headerLayout->addWidget( _symbolWidget );
				_headerLayout->addWidget( _enableWidget );
				_headerLayout->addSpacing( Style::INSPECTOR_HEADER_NAME_SPACING );
				_headerLayout->addWidget( _nameWidget );
				_headerLayout->addStretch( 1000 );

				_headerWidget->setFixedHeight( Style::INSPECTOR_HEADER_HEIGHT );

				_verticalLayout->addWidget( _headerWidget );

				_content = new QWidget( this );
				_content->setObjectName( "inspector_item_content" );
				_content->setContentsMargins( 0, 0, 0, 5 );
				_contentLayout = new QVBoxLayout( _content );
				_contentLayout->setContentsMargins( Style::INSPECTOR_CONTENT_HORIZONTAL_OFFSET + 10, 0, 10, 0 );

				_info = new QLabel( this );
				_info->setContentsMargins( 0, 0, 0, 0 );
				_info->setWordWrap( true );

				_contentLayout->addWidget( _info );

				_verticalLayout->addWidget( _content );
				_verticalLayout->addStretch( 1000 );
			}

			void InspectorMoleculeWidget::_setupSlots()
			{
				connect( _foldState, &QPushButton::clicked, this, &InspectorMoleculeWidget::toggleFoldState );
				connect( _enableWidget, &QCheckBox::stateChanged, this, &InspectorMoleculeWidget::setModelEnableFromCheckBox );
			};

			void InspectorMoleculeWidget::setFoldState( bool foldState )
			{
				_folded = foldState;
				refresh();
			}

			void InspectorMoleculeWidget::refresh()
			{
				if ( _model == nullptr )
					return;

				_foldState->setIcon( _folded ? VTX::Style::IconConst::get().FOLDED_PIXMAP : VTX::Style::IconConst::get().UNFOLDED_PIXMAP );
				_enableWidget->setCheckState( _model->isEnable() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
				_nameWidget->setText( QString::fromStdString( _model->getPdbIdCode() ) );

				if ( _folded )
					_content->hide();
				else
					_content->show();

				QString fullName = QString();
				fullName.append( "Full Name: " );
				fullName.append( QString::fromStdString( _model->getName() ) );
				_info->setText( fullName );
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
