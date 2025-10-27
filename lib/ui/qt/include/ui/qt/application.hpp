#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "settings.hpp"
#include "widget/main_window.hpp"
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include <app/vtx_app.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT
{
	/**
	 * @brief UI application class inheriting from QApplication and VTXApp.
	 */
	class Application final : public QApplication, public App::VTXApp
	{
	  public:
		/**
		 * @brief Construct the application with given arguments.
		 */
		Application( const App::Args & );

		/**
		 * @brief Destructor.
		 */
		~Application();

		/**
		 * @brief Run the main loop.
		 */
		void start() override;

		/**
		 * @brief Exit the main loop.
		 */
		void stop();

		/**
		 * @brief Handles exceptions in Qt event loop.
		 */
		bool notify( QObject * const, QEvent * const ) override;

	  private:
		/**
		 * @brief Handles the main loop timing.
		 */
		QTimer _timer;

		/**
		 * @brief Computes elapsed time and interval time between frames.
		 */
		VTX::Util::Chrono _durationTimer;

		/**
		 * @brief Load the graphical theme.
		 */
		void _loadTheme();

		/**
		 * @brief Get the QApplication instance.
		 */
		inline static QApplication * const Q_APP() { return qApp; }
	};
} // namespace VTX::UI::QT

#endif
