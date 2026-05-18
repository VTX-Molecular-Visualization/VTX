#include "ui/qt/widget/status_bar.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QMenu>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/services.hpp>
#include <app/vtx_app.hpp>
#include <renderer/renderer.hpp>
#include <array>
#include <string>
#include <string_view>
#include <util/event_hub.hpp>
#include <util/monitoring/stats.hpp>

namespace
{
	struct ResolutionPreset
	{
		std::string_view name;
		size_t			 width;
		size_t			 height;
	};

	constexpr std::array<ResolutionPreset, 11> RESOLUTION_PRESETS {
		ResolutionPreset { "SVGA", 800, 600 },
		ResolutionPreset { "XGA", 1024, 768 },
		ResolutionPreset { "HD", 1280, 720 },
		ResolutionPreset { "WXGA", 1280, 800 },
		ResolutionPreset { "SXGA", 1280, 1024 },
		ResolutionPreset { "HD+", 1600, 900 },
		ResolutionPreset { "WSXGA+", 1680, 1050 },
		ResolutionPreset { "Full HD", 1920, 1080 },
		ResolutionPreset { "WUXGA", 1920, 1200 },
		ResolutionPreset { "QHD", 2560, 1440 },
		ResolutionPreset { "4K UHD", 3840, 2160 },
	};
}

namespace VTX::UI::QT::Widget
{

	StatusBar::StatusBar( QWidget * p_parent ) : QStatusBar( p_parent )
	{
		_python = new QLabel( this );
		_python->setText( "No Python" );

		_fps = new QLabel( this );
		//_fps->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		_resolution = new QToolButton( this );
		_resolution->setAutoRaise( true );
		_resolution->setPopupMode( QToolButton::InstantPopup );
		_resolution->setToolButtonStyle( Qt::ToolButtonTextOnly );
		_setResolution( App::RENDERER().width(), App::RENDERER().height() );
		_setupResolutionMenu();

		auto * vendorLabel = new QLabel( this );
		vendorLabel->setText( "No renderer" );
		// vendorLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

		QWidget * spacer = new QWidget( this );
		spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

		QLabel * gpuIcon = new QLabel( this );
		gpuIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::GPU ).pixmap( 16, 16 ) );

		QLabel * resolutionIcon = new QLabel( this );
		resolutionIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::SCREEN_RESOLUTION ).pixmap( 16, 16 ) );

		QLabel * pythonIcon = new QLabel( this );
		pythonIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::PYTHON ).pixmap( 16, 16 ) );

		addPermanentWidget( spacer );
		addPermanentWidget( _fps );
		addPermanentWidget( resolutionIcon );
		addPermanentWidget( _resolution );
		addPermanentWidget( gpuIcon );
		addPermanentWidget( vendorLabel );
		addPermanentWidget( pythonIcon );
		addPermanentWidget( _python );

		// Update vendor when renderer is available.
		App::RENDERER().onReady += [ vendorLabel ]()
		{ vendorLabel->setText( QString::fromStdString( App::RENDERER().getInfos( true ).renderer ) ); };

		// Update FPS each second.
		auto * timer = new QTimer( this );
		connect( timer, &QTimer::timeout, this, &StatusBar::_setCurrentFPS );
		timer->start( 1000 );

		// Update renderering mode.
		App::HUB().connect<App::Events::PostRender, &StatusBar::_updateGPUState>( this );
		App::HUB().connect<App::Events::RendererResize, &StatusBar::_updateResolution>( this );
		App::HUB().connect<App::Events::PythonInitialized, &StatusBar::_pythonInitialized>( this );
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
			_fps->setText( QString( "-idle-" ) );
		}
	}

	void StatusBar::_setupResolutionMenu()
	{
		auto * const menu = new QMenu( _resolution );

		for ( const ResolutionPreset & preset : RESOLUTION_PRESETS )
		{
			QAction * const action = menu->addAction(
				QString( "%1 (%2x%3)" )
					.arg( QString::fromStdString( std::string( preset.name ) ) )
					.arg( preset.width )
					.arg( preset.height )
			);

			connect(
				action,
				&QAction::triggered,
				this,
				[ width = preset.width, height = preset.height ]()
				{ App::ACTION().execute<App::Action::Application::Resize>( width, height ); }
			);
		}

		_resolution->setMenu( menu );
	}

	void StatusBar::_setResolution( const size_t p_width, const size_t p_height )
	{
		_resolution->setText( QString( "%1x%2" ).arg( p_width ).arg( p_height ) );
		_resolution->setFixedWidth( _resolution->fontMetrics().horizontalAdvance( _resolution->text() ) + 8 );
	}

	void StatusBar::_updateResolution( const App::Events::RendererResize & p_e )
	{
		_setResolution( p_e.width, p_e.height );
	}

	void StatusBar::_setCurrentFPS()
	{
		if ( _rendering )
		{
			const float tickrate = App::ECS::getCtx<Util::Monitoring::Stats>().average();
			_fps->setText( QString( "%1 FPS" ).arg( uint( 1000.0 / tickrate ) ) );
		}
	}

	void StatusBar::_pythonInitialized( const App::Events::PythonInitialized & p_e )
	{
		_python->setText( QString( "Python %1" ).arg( QString::fromStdString( p_e.version ) ) );
	}

} // namespace VTX::UI::QT::Widget
