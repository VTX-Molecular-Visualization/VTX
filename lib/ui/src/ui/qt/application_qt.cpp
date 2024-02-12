#include "ui/qt/application_qt.hpp"
#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/visualization.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/tool/render/dialog.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QCoreApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QPalette>
#include <QStyleFactory>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/vtx_app.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	void ApplicationQt::configure()
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		// Init resources
		Q_INIT_RESOURCE( resources_ui );
	}

	int ZERO = 0;
	ApplicationQt::ApplicationQt() : Core::BaseUIApplication(), QApplication( ZERO, nullptr )
	{
		connect( this, &QCoreApplication::aboutToQuit, this, &ApplicationQt::stop );
	}
	ApplicationQt::~ApplicationQt() {}

	void ApplicationQt::init()
	{
		Core::BaseUIApplication::init();

		_inputManagerPtr = std::make_unique<Input::InputManager>();
		App::VTXApp::get().getSystem().referenceSystem( INPUT_MANAGER_KEY, _inputManagerPtr.get() );

		_currentMode = std::make_unique<Mode::Visualization>();
	}
	void ApplicationQt::start( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::start( p_args );
		_currentMode->enter();

		_handleArgs( p_args );
		_returnCode = exec();
	}

	void ApplicationQt::update()
	{
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		Core::BaseUIApplication::update();
	}

	void ApplicationQt::quit() { QApplication::quit(); };

	void ApplicationQt::_initUI( const std::vector<std::string> & p_args )
	{
		//// Init Modes.
		//_currentMode = Mode::ModeCollection::get().instantiateItem( _currentModeKey );
		_currentMode->enter();

		// Create UI.
		_initQt();

		_instantiateMainWindow();

		_mainWindow->getMainMenu().setCurrentTab( 0 );
		//_renderWidget->setFocus();

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &ApplicationQt::update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();
	}

	void ApplicationQt::_initQt()
	{
		setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		setStyle( QString::fromStdString( Style::DEFAULT_STYLE_FACTORY ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
	}

	void ApplicationQt::_instantiateMainWindow()
	{
		_mainWindow = WidgetFactory::get().instantiateWidget<QT::MainWindow>( nullptr, "MainWindow" );
	}

	void ApplicationQt::_postInit( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::_postInit( p_args );

		App::VTXApp::get().onPreRender().addCallback(
			this,
			[]( float p_deltaTime )
			{
				RendererQt qtRenderer = QT_RENDERER();

				qtRenderer.get().setMatrixView( App::SCENE().getCamera().getViewMatrix() );
				qtRenderer.get().setMatrixProjection( App::SCENE().getCamera().getProjectionMatrix() );
				qtRenderer.get().setCameraClipInfos(
					App::SCENE().getCamera().getNear(), App::SCENE().getCamera().getFar()
				);
			}
		);

		_mainWindow->show();
		_mainWindow->initWindowLayout();
	}

	void ApplicationQt::stop()
	{
		// _timer can be uninitialized if critical error append during start (i.e. OpenGL init fail)
		if ( _timer != nullptr )
		{
			_timer->stop();
			delete _timer;
		}

		// TODO BaseUIApplication::stop() called here because some model are strongly linked to _gl
		BaseUIApplication::stop();

		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}
	}

	bool ApplicationQt::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
#ifdef VTX_PRODUCTION
			UI::Dialog::unhandledException();
#endif
			return true;
		}
	}

	Input::InputManager & INPUT_MANAGER() { return QT_APP()->getInputManager(); }
	Mode::BaseMode &	  MODE() { return QT_APP()->getCurrentMode(); }
	RendererQt			  QT_RENDERER() { return RendererQt( App::RENDERER() ); };

} // namespace VTX::UI::QT
