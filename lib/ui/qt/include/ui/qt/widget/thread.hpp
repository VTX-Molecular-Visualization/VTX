#ifndef __VTX_UI_QT_WIDGET_THREAD__
#define __VTX_UI_QT_WIDGET_THREAD__

#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget to display a thread progress.
	 */
	class Thread : public QWidget
	{
		Q_OBJECT

	  public:
		Thread( QWidget * p_parent )
		{
			auto * layout = new QHBoxLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			_progressBar = new _ProgressBar( this );
			_progressBar->setTextVisible( true );

			_cancelButton = new QPushButton( this );
			_cancelButton->setIcon( STYLE().iconFromCodepoint( Style::Icons::CANCEL ) );
			_cancelButton->setFlat( true );

			connect( _cancelButton, &QPushButton::clicked, this, [ this ]() { emit cancelClicked(); } );

			layout->addWidget( _cancelButton );
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

	  signals:
		void cancelClicked();

	  private:
		/**
		 * @brief Override progress bar to display text when indeterminate.
		 */
		class _ProgressBar : public QProgressBar
		{
		  public:
			using QProgressBar::QProgressBar;

			void paintEvent( QPaintEvent * p_event ) override
			{
				QProgressBar::paintEvent( p_event );

				if ( minimum() == 0 && maximum() == 0 )
				{
					QPainter painter( this );
					painter.drawText( rect(), Qt::AlignCenter, format() );
				}
			}
		};

		QPointer<_ProgressBar> _progressBar;
		QPointer<QPushButton>  _cancelButton;
	};
} // namespace VTX::UI::QT::Widget

#endif
