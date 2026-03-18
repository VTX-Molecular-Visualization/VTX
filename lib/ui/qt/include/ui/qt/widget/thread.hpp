#ifndef __VTX_UI_QT_WIDGET_THREAD__
#define __VTX_UI_QT_WIDGET_THREAD__

#include <QLabel>
#include <QPointer>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to display a thread progress.
	 */
	class Thread : public QWidget
	{
	  public:
		Thread( QWidget * p_parent )
		{
			auto * layout = new QVBoxLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			_progressBar = new QProgressBar( this );
			_progressBar->setTextVisible( true );

			layout->addWidget( _progressBar );
		}

		void set( const QString & p_text, const float p_progress )
		{
			if ( p_progress <= 0.f )
			{
				_progressBar->setRange( 0, 0 );
			}
			else
			{
				_progressBar->setRange( 0, 100 );
			}
			_progressBar->setValue( static_cast<int>( p_progress * 100.f ) );
			_progressBar->setFormat( p_text );
		}

	  private:
		QPointer<QProgressBar> _progressBar;
	};
} // namespace VTX::UI::QT::Widget

#endif
