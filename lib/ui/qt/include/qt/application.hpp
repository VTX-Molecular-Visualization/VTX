#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "main_window.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QPointer>
#include <QSettings>
#include <QSplashScreen>
#include <QTimer>
#include <app/filesystem.hpp>
#include <ui/base_application.hpp>

namespace VTX::UI::QT
{
	inline QSettings SETTINGS
		= QSettings( QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat );

	class Application final : public UI::BaseApplication<MainWindow>, QApplication
	{
	  public:
		Application();
		~Application();

		static void configure();

		// bool event( QEvent * ) override;
		// Check exception in Qt events.
		bool notify( QObject * const, QEvent * const ) override;

	  protected:
		// Override BaseApplication.
		void _start() override;

	  private:
		QTimer					_timer;
		QElapsedTimer			_elapsedTimer;
		QPointer<QSplashScreen> _qSplashScreen;

		void _loadTheme();
		void _saveSettings();
		void _restoreSettings();
	};
} // namespace VTX::UI::QT

#endif
