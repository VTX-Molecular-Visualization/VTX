#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "core/base_ui_application.hpp"
#include "environment.hpp"
#include "qt/main_window.hpp"
#include "qt/state/state_machine.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QObject>
#include <QTimer>
#include <util/types.hpp>

namespace VTX::UI::QT
{
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

		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const;

		inline MainWindow &				   getMainWindow() { return *_mainWindow; }
		inline const MainWindow &		   getMainWindow() const { return *_mainWindow; }
		inline State::StateMachine &	   getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine & getStateMachine() const { return *_stateMachine; }

		bool notify( QObject * const, QEvent * const ) override;

	  protected:
		void _initUI( const std::vector<std::string> & p_args ) override;
		void _postInit( const std::vector<std::string> & p_args ) override;

		void _initQt();
		void _instantiateMainWindow();

	  private:
		MainWindow * _mainWindow = nullptr;

		State::StateMachine * _stateMachine = nullptr;

		QTimer *	  _timer		= nullptr;
		QElapsedTimer _elapsedTimer = QElapsedTimer();
		QElapsedTimer _tickTimer	= QElapsedTimer();
		uint		  _tickCounter	= 0u;
	};

	inline ApplicationQt * const QT_APP() { return VTX::UI::Environment::get().getUIApp<VTX::UI::QT::ApplicationQt>(); }
} // namespace VTX::UI::QT

#endif
