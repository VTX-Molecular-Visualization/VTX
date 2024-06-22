#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include <ui/core/base_ui_application.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	class QApplication;
	class MainWindow;

	class ApplicationQt : public UI::Core::BaseUIApplication //, public QApplication
	{
	  public:
		// static void configure();

	  private:
		inline static const std::string INPUT_MANAGER_KEY = "INPUT_MANAGER";

	  public:
		ApplicationQt();
		ApplicationQt( const ApplicationQt & )			   = delete;
		ApplicationQt & operator=( const ApplicationQt & ) = delete;
		~ApplicationQt();

		void start( const std::vector<std::string> & p_args ) override;
		void stop() override;

		inline MainWindow &		  getMainWindow() { return *_mainWindow; }
		inline const MainWindow & getMainWindow() const { return *_mainWindow; }

		// bool notify( QObject * const, QEvent * const ) override;

		void softQuit();

	  protected:
		void _initUI( const std::vector<std::string> & p_args ) override;
		void _startUI( const std::vector<std::string> & p_args ) override;

		void _initQt();
		void _instantiateMainWindow();

	  private:
		QApplication * _qApplication = nullptr;
		MainWindow *   _mainWindow	 = nullptr;
	};

	inline ApplicationQt * const QT_APP() { return &Util::Generic::UniqueInstance<ApplicationQt>::get(); }

} // namespace VTX::UI::QT

#endif
