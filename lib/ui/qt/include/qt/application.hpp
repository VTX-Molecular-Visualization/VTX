#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "main_window.hpp"
#include <QPointer>
#include <ui/base_application.hpp>

class QMenu;
class QToolBar;
class QApplication;
class QSplashScreen;

namespace VTX::UI::QT
{

	class Application : public UI::BaseApplication<MainWindow>
	{
	  public:
		Application();
		virtual ~Application() {}

		// Add widgets.
		void addMenu( QMenu * const );
		void addToolBar( QToolBar * const );
		void addDockWidget( Qt::DockWidgetArea, QDockWidget * const );
		void addDialog( QDialog * const );

		// Create widgets.
		template<class M>
		void createMenu()
		{
			_mainWindow->menuBar()->addMenu( new M( _mainWindow ) );
		}

		template<class T>
		void createToolBar()
		{
			_mainWindow->menuBar()->addToolBar( new T( _mainWindow ) );
		}

	  protected:
		// Override.
		void _init( const App::Args & ) override;
		void _start() override;
		void _stop() override;

	  private:
		QPointer<QApplication>	_qApplication;
		QPointer<QSplashScreen> _qSplashScreen;
		void					_loadTheme();
	};

} // namespace VTX::UI::QT

namespace VTX
{
	inline UI::QT::Application & APP_QT() { return static_cast<UI::QT::Application &>( APP() ); }
} // namespace VTX

#endif
