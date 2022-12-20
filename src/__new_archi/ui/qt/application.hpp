#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include "__new_archi/ui/core/application.hpp"
#include "__new_archi/ui/environment.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QObject>
#include <QTimer>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	class ApplicationQt : public Core::Application, public QApplication
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

		MainWindow & getMainWindow() const { return *_mainWindow; }

		bool notify( QObject * const, QEvent * const ) override;

	  protected:
		void _initUI( const std::vector<std::string> & p_args ) override;

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
