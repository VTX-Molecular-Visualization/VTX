#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "main_window.hpp"
#include <QPointer>
#include <ui/base_application.hpp>

class QApplication;
class QMenu;
class QToolBar;

namespace VTX::UI::QT
{

	class Application : public UI::BaseApplication<MainWindow>
	{
	  public:
		Application() : UI::BaseApplication<MainWindow>() { VTX_DEBUG( "Application()" ); }
		virtual ~Application() {}

		// Add widgets.
		void addMenu( QMenu * const p_menu );
		void addToolBar( QToolBar * const p_toolBar );
		void addDockWidget( QDockWidget * const p_dockWidget );
		void addDialog( QDialog * const p_dialog );

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
		void _init( const App::Args & p_args ) override;
		void _start() override;
		void _stop() override;

	  private:
		QPointer<QApplication> _qApplication;

		void _loadTheme();
	};

} // namespace VTX::UI::QT

namespace VTX
{
	inline UI::QT::Application & APP_QT() { return static_cast<UI::QT::Application &>( APP() ); }
} // namespace VTX

#endif
