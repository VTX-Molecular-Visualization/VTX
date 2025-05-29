#include "ui/qt/widget/status_bar.hpp"
#include "app/monitoring/constants.hpp"
#include <app/core/renderer/renderer_system.hpp>
#include <app/vtx_app.hpp>
#include <util/monitoring/stats.hpp>

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
		App::RENDERER_SYSTEM().onReady() += [ vendorLabel ]()
		{
			//
			vendorLabel->setText( QString::fromStdString( App::RENDERER_SYSTEM().getInfos().renderer ) );
		};

		// Update FPS each second.
		auto * timer = new QTimer( this );
		connect(
			timer,
			&QTimer::timeout,
			this,
			[ this, fpsLabel ]()
			{
				const double tickrate = App::STATS().getAverage<double>( App::Monitoring::TICK_RATE_KEY );

				fpsLabel->setText( QString( "%1 FPS" ).arg( uint( 1000.0 / tickrate ) ) );
			}
		);
		timer->start( 1000 );
	}

} // namespace VTX::UI::QT::Widget
