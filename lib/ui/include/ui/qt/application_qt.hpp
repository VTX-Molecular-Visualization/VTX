#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "ui/core/base_ui_application.hpp"
#include "ui/environment.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QObject>
#include <QTimer>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	namespace State
	{
		class StateMachine;
	}
	class MainWindow;

	class ApplicationQt : public Core::BaseUIApplication, public QApplication
	{
	  public:
		static void configure();

	  public:
		ApplicationQt();
		ApplicationQt( const ApplicationQt & )			   = delete;
		ApplicationQt & operator=( const ApplicationQt & ) = delete;
		~ApplicationQt();

		void init() override;
		void start( const std::vector<std::string> & p_args ) override;
		void update() override;
		void stop() override;
		void quit() override;

		inline MainWindow &		  getMainWindow() { return *_mainWindow; }
		inline const MainWindow & getMainWindow() const { return *_mainWindow; }

		bool notify( QObject * const, QEvent * const ) override;

	  protected:
		void _initUI( const std::vector<std::string> & p_args ) override;
		void _postInit( const std::vector<std::string> & p_args ) override;

		void _initQt();
		void _instantiateMainWindow();

	  private:
		MainWindow * _mainWindow = nullptr;

		QTimer *	  _timer		= nullptr;
		QElapsedTimer _elapsedTimer = QElapsedTimer();
		QElapsedTimer _tickTimer	= QElapsedTimer();
		uint		  _tickCounter	= 0u;
	};

	inline ApplicationQt * const QT_APP() { return VTX::UI::Environment::get().getUIApp<VTX::UI::QT::ApplicationQt>(); }
} // namespace VTX::UI::QT

#endif
