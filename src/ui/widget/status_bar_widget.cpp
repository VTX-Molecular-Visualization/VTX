#include "status_bar_widget.hpp"
#include "vtx_app.hpp"
#include <QHBoxLayout>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			StatusBarWidget::StatusBarWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _registerEvent( Event::Global::UPDATE_PROGRESS_BAR ); }

			void StatusBarWidget ::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::UPDATE_PROGRESS_BAR )
				{
					float progress = dynamic_cast<const Event::VTXEventValue<float> &>( p_event ).value;

					if ( progress == 1.0f )
					{
						_progressBar->hide();
					}
					else
					{
						int progressBarValue = (int)( progress * VTX_SETTING().STATUS_PROGRESS_BAR_CHUNKS );
						_progressBar->setValue( progressBarValue );
						_progressBar->show();
					}
				}
			}

			void StatusBarWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );

				_infoLabel = new QLabel( this );
				_infoLabel->setText( "Status bar" );
				this->addWidget( _infoLabel );

				_progressBar = new QProgressBar( this );
				_progressBar->setMinimum( 0 );
				_progressBar->setMaximum( VTX_SETTING().STATUS_PROGRESS_BAR_CHUNKS );
				_progressBar->setTextVisible( false );
				_progressBar->setFixedWidth( VTX_SETTING().STATUS_PROGRESS_BAR_WIDTH );
				_progressBar->hide();
				this->addWidget( _progressBar );
			}

			void StatusBarWidget::_setupSlots() {}
			void StatusBarWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
