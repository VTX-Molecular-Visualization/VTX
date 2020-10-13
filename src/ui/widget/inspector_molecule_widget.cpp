#include "inspector_molecule_widget.hpp"
#include "ui/ui_const.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void InspectorMoleculeWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );

				_verticalLayout = new QVBoxLayout( this );
				_verticalLayout->setContentsMargins( 0, 0, 0, 0 );

				_headerWidget = new QWidget( this );
				_headerWidget->setSizePolicy( QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum );
				_headerLayout = new QHBoxLayout( _headerWidget );

				_foldState = new QPushButton( _headerWidget );
				_foldState->setContentsMargins( 0, 0, 0, 0 );
				_enableWidget = new QCheckBox( _headerWidget );

				_headerLayout->addWidget( _foldState );
				_headerLayout->addWidget( _enableWidget );

				_verticalLayout->addWidget( _headerWidget );

				_content = new QWidget( this );
				_content->setSizePolicy( QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum );
				_contentLayout = new QVBoxLayout( _content );

				_info = new QLabel( this );
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

				_foldState->setIcon( _folded ? UiConsts::get().FOLDED_PIXMAP : UiConsts::get().UNFOLDED_PIXMAP );

				_enableWidget->setCheckState( _model->isEnable() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
				_enableWidget->setText( QString::fromStdString( _model->getPdbIdCode() ) );

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
