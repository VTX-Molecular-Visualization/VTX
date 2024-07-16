#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "widget/main_window.hpp"
#include <QApplication>
#include <QPointer>
#include <QSettings>
#include <QSplashScreen>
#include <QTimer>
#include <app/filesystem.hpp>
#include <ui/base_application.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{
	inline QSettings SETTINGS
		= QSettings( QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat );

	class Application final : public UI::BaseApplication<Widget::MainWindow>, QApplication
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
		Util::Chrono			_elapsedTimer;
		Util::Chrono			_deltaTimer;
		QPointer<QSplashScreen> _qSplashScreen;

		void _loadTheme();
		void _saveSettings();
		void _restoreSettings();
	};
} // namespace VTX::UI::QT

#endif
