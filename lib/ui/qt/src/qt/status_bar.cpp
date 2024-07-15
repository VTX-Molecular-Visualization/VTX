#include "qt/status_bar.hpp"
#include "app/internal/monitoring/all_metrics.hpp"
#include <app/application/system/renderer.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::QT
{

	StatusBar::StatusBar( QWidget * p_parent ) : BaseWidget<StatusBar, QStatusBar>( p_parent )
	{
		auto * vendorLabel = new QLabel( this );
		vendorLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		auto * fpsLabel = new QLabel( this );
		fpsLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		// QWidget * spacer = new QWidget( this );
		// spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

		// addPermanentWidget( spacer );
		addPermanentWidget( vendorLabel );
		addPermanentWidget( fpsLabel );

		// Update vendor when renderer is available.
		auto & renderer = App::RENDERER_SYSTEM().facade();
		renderer.onReady() += [ vendorLabel, &renderer ]()
		{ vendorLabel->setText( QString::fromStdString( renderer.getInfos().renderer ) ); };

		// Update FPS each second.
		auto * timer = new QTimer( this );
		connect(
			timer,
			&QTimer::timeout,
			this,
			[ this, fpsLabel ]()
			{
				const float tickrate = App::STATS().getAverage<float>( App::Internal::Monitoring::TICK_RATE_KEY );

				fpsLabel->setText( QString( "FPS: %1" ).arg( uint( 1.f / tickrate ) ) );
			}
		);
		timer->start( 1000 );
	}

} // namespace VTX::UI::QT
