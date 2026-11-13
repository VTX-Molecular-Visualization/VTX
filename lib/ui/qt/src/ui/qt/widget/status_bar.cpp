#include "ui/qt/widget/status_bar.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QMenu>
#include <app/action/application.hpp>
#include <app/services.hpp>
#include <renderer/renderer.hpp>
#include <util/action_manager.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{

	StatusBar::StatusBar( QWidget * const p_parent ) : QStatusBar( p_parent )
	{
		// Python status.
		_python = Helper::makeQObject<QLabel>( this );
		_python->setText( "No Python" );

		// FPS status.
		_fps = Helper::makeQObject<QLabel>( this );

		// Resolution widget.
		_resolution = Helper::makeQObject<ToolButton::ResolutionSelector>( this );
		_resolution->setResolution( App::RENDERER().width(), App::RENDERER().height() );
		_resolution->setAutoRaise( false );
		_resolution->setToolTip( "Change resolution" );
		_resolution->setStatusTip( "Change the resolution of the renderer" );
		connect(
			_resolution,
			&ToolButton::ResolutionSelector::resolutionChanged,
			this,
			[]( const size_t width, const size_t height )
			{ App::ACTION().execute<App::Action::Application::Resize>( width, height ); }
		);

		// Labels.
		_vendor = Helper::makeQObject<QLabel>( this );
		_vendor->setText( "No renderer" );

		QWidget * spacer = Helper::makeQObject<QWidget>( this );
		spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

		QLabel * gpuIcon = Helper::makeQObject<QLabel>( this );
		gpuIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::GPU ).pixmap( 16, 16 ) );

		QLabel * resolutionIcon = Helper::makeQObject<QLabel>( this );
		resolutionIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::SCREEN_RESOLUTION ).pixmap( 16, 16 ) );

		QLabel * pythonIcon = Helper::makeQObject<QLabel>( this );
		pythonIcon->setPixmap( STYLE().iconFromCodepoint( Style::Icons::PYTHON ).pixmap( 16, 16 ) );

		addPermanentWidget( spacer );
		addPermanentWidget( _fps );
		addPermanentWidget( resolutionIcon );
		addPermanentWidget( _resolution );
		addPermanentWidget( gpuIcon );
		addPermanentWidget( _vendor );
		addPermanentWidget( pythonIcon );
		addPermanentWidget( _python );

		// Update vendor when renderer is available.
		_rendererReadyConnection = App::HUB().connect<App::Events::RendererReady, &StatusBar::_rendererReady>( this );

		// Update renderering mode.
		_postRenderConnection = App::HUB().connect<App::Events::PostRender, &StatusBar::_updateGPUState>( this );
		_rendererResizeConnection
			= App::HUB().connect<App::Events::RendererResize, &StatusBar::_updateResolution>( this );
		_pythonInitializedConnection
			= App::HUB().connect<App::Events::PythonInitialized, &StatusBar::_pythonInitialized>( this );
	}

	void StatusBar::_updateGPUState( const App::Events::PostRender & p_e )
	{
		if ( p_e.rendered )
		{
			if ( not _rendering )
			{
				_rendering	= true;
				_frameCount = 0;
				_fpsChrono.start();
			}
			++_frameCount;
			_setCurrentFPS();
		}

		if ( p_e.idle && _rendering )
		{
			_rendering = false;
			_fps->setText( QString( "-idle-" ) );
		}
	}

	void StatusBar::_updateResolution( const App::Events::RendererResize & p_e )
	{
		_resolution->setResolution( p_e.width, p_e.height );
		_resolution->setFixedWidth( _resolution->fontMetrics().horizontalAdvance( _resolution->text() ) + 12 );
	}

	void StatusBar::_setCurrentFPS()
	{
		const float elapsed = _fpsChrono.elapsedTime();
		if ( elapsed < FPS_REFRESH_INTERVAL )
		{
			return;
		}

		_fps->setText(
			QString( "%1 FPS" ).arg( static_cast<uint>( static_cast<float>( _frameCount ) * 1000.f / elapsed ) )
		);
		_frameCount = 0;
		_fpsChrono.start();
	}

	void StatusBar::_pythonInitialized( const App::Events::PythonInitialized & p_e )
	{ _python->setText( QString( "Python %1" ).arg( QString::fromStdString( p_e.version ) ) ); }

	void StatusBar::_rendererReady( const App::Events::RendererReady & p_e )
	{ _vendor->setText( QString::fromStdString( p_e.infos.renderer ) ); }

} // namespace VTX::UI::QT::Widget
