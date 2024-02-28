#include "status_bar_widget.hpp"
#include "vtx_app.hpp"
#include <QHBoxLayout>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace StatusBar
			{
				StatusBarWidget::StatusBarWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::UPDATE_PROGRESS_BAR );
					_registerEvent( Event::Global::UPDATE_STATUS_BAR );
				}

				void StatusBarWidget ::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::UPDATE_PROGRESS_BAR )
					{
						/*
						float progress = dynamic_cast<const Event::VTXEventValue<float> &>( p_event ).value;

						if ( progress == 1.0f )
						{
							_progressBar->hide();
						}
						else
						{
							int progressBarValue = (int)( progress * Style::STATUS_PROGRESS_BAR_CHUNKS );
							_progressBar->setValue( progressBarValue );
							_progressBar->show();
						}
						*/

						bool active = dynamic_cast<const Event::VTXEventValue<bool> &>( p_event ).value;
						if ( active )
						{
							_infoLabel->setText( "Busy" );
							_progressBar->show();
						}
						else
						{
							_infoLabel->setText( "Ready" );
							_progressBar->hide();
						}
					}
					else if ( p_event.name == Event::Global::UPDATE_STATUS_BAR )
					{
						_infoLabel->setText( QString::fromStdString(
							dynamic_cast<const Event::VTXEventValue<std::string> &>( p_event ).value ) );
					}
				}

				void StatusBarWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_progressBar = new QProgressBar( this );
					_progressBar->setMinimum( 0 );
					//_progressBar->setMaximum( Style::STATUS_PROGRESS_BAR_CHUNKS );
					_progressBar->setValue( 0 );
					_progressBar->setMaximum( 0 );
					_progressBar->setTextVisible( false );
					_progressBar->setFixedWidth( Style::STATUS_PROGRESS_BAR_WIDTH );
					_progressBar->hide();
					this->addWidget( _progressBar );

					_infoLabel = new QLabel( this );
					_infoLabel->setText( "Ready" );
					this->addWidget( _infoLabel );
				}

				void StatusBarWidget::_setupSlots() {}
				void StatusBarWidget::localize() {}
			} // namespace StatusBar
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
