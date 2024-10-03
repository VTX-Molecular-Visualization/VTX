#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "settings.hpp"
#include "widget/main_window.hpp"
#include <QApplication>
#include <QPointer>
#include <QSplashScreen>
#include <QTimer>
#include <app/ui/base_application.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{
	class Application final : public App::UI::BaseApplication<Widget::MainWindow>, public QApplication
	{
	  public:
		Application();
		~Application();

		static void configure();

		// bool event( QEvent * ) override;
		//  Check exception in Qt events.
		bool notify( QObject * const, QEvent * const ) override;

	  protected:
		// Override BaseApplication.
		void _start() override;

	  private:
		QTimer					_timer;
		VTX::Util::Chrono		_elapsedTimer;
		VTX::Util::Chrono		_deltaTimer;
		QPointer<QSplashScreen> _qSplashScreen;

		void _loadTheme();
	};
} // namespace VTX::UI::QT

namespace VTX
{
	using APP_QT = UI::QT::Application;
} // namespace VTX

#endif
