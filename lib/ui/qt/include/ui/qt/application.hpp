#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "settings.hpp"
#include <QAction>
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include <app/ui/concepts.hpp>
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

		/**
		 * @brief Get app action from description.
		 */

		static inline QAction * const getAction( const App::UI::DescAction & p_action )
		{
			return _getOrCreateAction( p_action );
		}

		/**
		 * @brief Get app action from type.
		 */
		template<App::UI::ConceptAction A>
		static inline QAction * getAction()
		{
			A action;
			action.key = VTX::Util::typeName<A>();
			return _getOrCreateAction( action );
		}

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
		 * @brief Get/create application action from description.
		 */
		static QAction * const _getOrCreateAction( const App::UI::DescAction & );
	};

	/**
	 * @brief Get the QApplication instance.
	 */
	inline static QApplication * const Q_APP() { return qApp; }
} // namespace VTX::UI::QT

#endif
