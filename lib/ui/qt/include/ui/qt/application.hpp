#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "QSplashScreen"
#include "settings.hpp"
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include <app/events.hpp>
#include <app/vtx_app.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{

	/**
	 * @brief UI application class owning a QApplication shell and a VTXApp core instance.
	 */
	class Application final : public QApplication
	{
	  public:
		/**
		 * @brief Construct the application with given arguments.
		 */
		Application(  App::Arguments && );

		/**
		 * @brief Destructor.
		 */
		~Application();

		/**
		 * @brief Run the main loop.
		 */
		void start();

		/**
		 * @brief Exit the main loop.
		 */
		void stop();

		/**
		 * @brief Handles exceptions in Qt event loop.
		 */
		bool notify( QObject * const, QEvent * const ) override;

		/**
		 * @brief Forward tool.
		 */
		template<typename T>
		void addTool()
		{
			_app.addTool<T>();
		}

	  private:
		/**
		 * @brief Application core instance.
		 */
		App::VTXApp _app;

		/**
		 * @brief Handles the main loop timing.
		 */
		QTimer _timer;

		/**
		 * @brief Computes elapsed time and interval time between frames.
		 */
		VTX::Util::Chrono _durationTimer;

		/**
		 * @brief Splash screen displayed during startup.
		 */
		QPointer<QSplashScreen> _splashScreen;

		/**
		 * @brief Init application after Qt startup (create main window, OpenGL context, etc.).
		 */
		void _postQtStartup();

	};

	/**
	 * @brief Get the QApplication instance.
	 */
	inline static QApplication * const Q_APP() { return qApp; }
} // namespace VTX::UI::QT

#endif
