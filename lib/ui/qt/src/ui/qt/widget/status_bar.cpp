#include "ui/qt/widget/status_bar.hpp"
#include "app/services.hpp"
#include <app/vtx_app.hpp>
#include <renderer/facade.hpp>
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
		App::RENDERER().onReady(
			[ vendorLabel ]()
			{
				//
				vendorLabel->setText( QString::fromStdString( App::RENDERER().getInfos().renderer ) );
			}
		);

		// Update FPS each second.
		auto * timer = new QTimer( this );
		connect(
			timer,
			&QTimer::timeout,
			this,
			[ this, fpsLabel ]()
			{
				const float tickrate = App::ECS::getCtx<Util::Monitoring::Stats>().average();
				fpsLabel->setText( QString( "%1 FPS" ).arg( uint( 1000.0 / tickrate ) ) );
			}
		);
		timer->start( 1000 );
	}

} // namespace VTX::UI::QT::Widget
