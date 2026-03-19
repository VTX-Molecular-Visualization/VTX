#ifndef __VTX_UI_QT_WIDGET_SPLASH_SCREEN__
#define __VTX_UI_QT_WIDGET_SPLASH_SCREEN__

#include "ui/qt/resources.hpp"
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QPointer>
#include <QScreen>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom splash screen to draw a GIF as movie.
	 */
	class SplashScreen : public QWidget
	{
	  public:
		SplashScreen( QWidget * p_parent = nullptr ) : QWidget( nullptr, Qt::SplashScreen | Qt::FramelessWindowHint )
		{
			using namespace Resources;

			setAttribute( Qt::WA_DeleteOnClose, false );
			setObjectName( "StartupSplashScreen" );

			auto * layout = new QHBoxLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			_splashLabel = new QLabel( this );
			_splashLabel->setAlignment( Qt::AlignCenter );
			layout->addWidget( _splashLabel );

			_splashMovie = new QMovie( QString::fromStdString( std::string( SPRITE_SPLASH_GIF ) ), QByteArray(), this );
			if ( _splashMovie->isValid() )
			{
				_splashLabel->setMovie( _splashMovie );
				_splashMovie->start();
				resize( _splashMovie->frameRect().size() );
			}
			else
			{
				_splashLabel->setPixmap( QPixmap( SPRITE_SPLASH.data() ) );
				adjustSize();
			}

			if ( const QScreen * screen = QGuiApplication::primaryScreen() )
			{
				const QRect geometry = screen->availableGeometry();
				move( geometry.center() - rect().center() );
			}
		}

		void stop() const
		{
			if ( _splashMovie )
			{
				_splashMovie->stop();
			}
		}

	  private:
		QPointer<QLabel>  _splashLabel;
		QPointer<QMovie>  _splashMovie;
	};
} // namespace VTX::UI::QT::Widget
#endif
