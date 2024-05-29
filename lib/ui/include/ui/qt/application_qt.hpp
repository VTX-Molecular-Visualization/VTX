#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "ui/core/base_ui_application.hpp"
#include "ui/environment.hpp"
#include "ui/qt/input/_fwd.hpp"
#include "ui/qt/mode/_fwd.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QObject>
#include <QTimer>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	class MainWindow;

	class ApplicationQt : public UI::Core::BaseUIApplication, public QApplication
	{
	  public:
		static void configure();

	  private:
		inline static const std::string INPUT_MANAGER_KEY = "INPUT_MANAGER";

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

		inline Mode::BaseMode &		  getCurrentMode() { return *_currentMode; }
		inline const Mode::BaseMode & getCurrentMode() const { return *_currentMode; }

		bool notify( QObject * const, QEvent * const ) override;

		void softQuit();

	  protected:
		void _initUI( const std::vector<std::string> & p_args ) override;
		void _startUI( const std::vector<std::string> & p_args ) override;

		void _initQt();
		void _instantiateMainWindow();

	  private:
		MainWindow * _mainWindow = nullptr;

		QTimer *	  _timer		= nullptr;
		QElapsedTimer _elapsedTimer = QElapsedTimer();
		QElapsedTimer _tickTimer	= QElapsedTimer();
		uint		  _tickCounter	= 0u;

		std::unique_ptr<Mode::BaseMode> _currentMode;
		std::string						_currentModeKey = "MODE_VISUALIZATION";
	};

	inline ApplicationQt * const QT_APP() { return VTX::UI::Environment::get().getUIApp<VTX::UI::QT::ApplicationQt>(); }
	Mode::BaseMode &			 MODE();

} // namespace VTX::UI::QT

#endif
