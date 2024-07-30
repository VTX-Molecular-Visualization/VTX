#include "qt/widget/status_bar.hpp"
#include "app/internal/monitoring/all_metrics.hpp"
#include <app/application/system/renderer.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::QT::Widget
{

	StatusBar::StatusBar( QWidget * p_parent ) : BaseWidget<StatusBar, QStatusBar>( p_parent )
	{
		auto * fpsLabel = new QLabel( this );
		fpsLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		auto * vendorLabel = new QLabel( this );
		vendorLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		// QWidget * spacer = new QWidget( this );
		// spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

		// addPermanentWidget( spacer );
		addPermanentWidget( fpsLabel );
		addPermanentWidget( vendorLabel );

		// Update vendor when renderer is available.
		auto & renderer = App::RENDERER().facade();
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

				fpsLabel->setText( QString( "%1 FPS" ).arg( uint( 1.f / tickrate ) ) );
			}
		);
		timer->start( 1000 );
	}

} // namespace VTX::UI::QT::Widget