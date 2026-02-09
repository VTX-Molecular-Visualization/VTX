#include "ui/qt/widget/status_bar.hpp"
#include "app/services.hpp"
#include <app/services.hpp>
#include <app/vtx_app.hpp>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
#include <util/monitoring/stats.hpp>

namespace VTX::UI::QT::Widget
{

	StatusBar::StatusBar( QWidget * p_parent ) : QStatusBar( p_parent )
	{
		_label = new QLabel( this );
		_label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		auto * vendorLabel = new QLabel( this );
		vendorLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		QWidget * spacer = new QWidget( this );
		spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

		addPermanentWidget( spacer );
		addPermanentWidget( _label );
		addPermanentWidget( vendorLabel );

		// Update vendor when renderer is available.
		App::RENDERER().onReady += [ vendorLabel ]()
		{ vendorLabel->setText( QString::fromStdString( App::RENDERER().getInfos( true ).renderer ) ); };

		// Update FPS each second.
		auto * timer = new QTimer( this );
		connect( timer, &QTimer::timeout, this, &StatusBar::_setCurrentFPS );
		timer->start( 1000 );

		// Update renderering mode.
		App::HUB().connect<App::Events::PostRender, &StatusBar::_updateGPUState>( this );
	}

	void StatusBar::_updateGPUState( const App::Events::PostRender & p_e )
	{
		if ( _rendering == p_e.rendered )
		{
			return;
		}

		_rendering = p_e.rendered;

		if ( not _rendering )
		{
			_label->setText( QString( "-idle-" ) );
		}
	}

	void StatusBar::_setCurrentFPS()
	{
		if ( _rendering )
		{
			const float tickrate = App::ECS::getCtx<Util::Monitoring::Stats>().average();
			_label->setText( QString( "%1 FPS" ).arg( uint( 1000.0 / tickrate ) ) );
		}
	}

} // namespace VTX::UI::QT::Widget
