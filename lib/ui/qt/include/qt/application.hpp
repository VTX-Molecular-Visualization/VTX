#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "main_window.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QPointer>
#include <QSettings>
#include <QTimer>
#include <ui/base_application.hpp>

class QMenu;
class QToolBar;
class QSplashScreen;

namespace VTX::UI::QT
{

	class Application final : public UI::BaseApplication<MainWindow>, QApplication
	{
	  public:
		Application();
		~Application();

		static void configure();

	  protected:
		// Override BaseApplication.
		void _start() override;

	  private:
		QPointer<QSplashScreen> _qSplashScreen;

		QSettings	  _settings;
		QTimer		  _timer;
		QElapsedTimer _elapsedTimer;

		void _loadTheme();
		void _saveSettings();
		void _restoreSettings();
	};

} // namespace VTX::UI::QT

#endif
