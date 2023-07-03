#include "ui/old_ui/ui/widget/status_bar/status_bar_widget.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QHBoxLayout>
#include <app/old/event/global.hpp>

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
					_registerEvent( VTX::App::Old::Event::Global::UPDATE_PROGRESS_BAR );
				}

				void StatusBarWidget ::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
				{
					if ( p_event.name == VTX::App::Old::Event::Global::UPDATE_PROGRESS_BAR )
					{
						const float progress
							= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<float> &>( p_event ).get();

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
					_progressBar->setMaximum( Style::STATUS_PROGRESS_BAR_CHUNKS );
					_progressBar->setTextVisible( false );
					_progressBar->setFixedWidth( Style::STATUS_PROGRESS_BAR_WIDTH );
					_progressBar->hide();
					this->addWidget( _progressBar );
				}

				void StatusBarWidget::_setupSlots() {}
				void StatusBarWidget::localize() {}
			} // namespace StatusBar
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
