#ifndef __VTX_UI_QT_APPLICATION__
#define __VTX_UI_QT_APPLICATION__

#include <ui/core/base_ui_application.hpp>
#include <util/types.hpp>

class QApplication;

namespace VTX::UI::QT
{
	class MainWindow;

	class ApplicationQt : public UI::Core::BaseUIApplication
	{
	  private:
		inline static const std::string INPUT_MANAGER_KEY = "INPUT_MANAGER";

	  public:
		ApplicationQt();
		ApplicationQt( const ApplicationQt & )			   = delete;
		ApplicationQt & operator=( const ApplicationQt & ) = delete;
		~ApplicationQt();

		inline MainWindow &		  getMainWindow() { return *_mainWindow; }
		inline const MainWindow & getMainWindow() const { return *_mainWindow; }

		// bool notify( QObject * const, QEvent * const ) override;

		void softQuit();

	  protected:
		void _init( const std::vector<std::string> & p_args ) override;
		void _build( const UI::Core::LayoutDescriptor & p_layout ) override;
		void _start( const std::vector<std::string> & p_args ) override;

	  private:
		QApplication * _qApplication;
		MainWindow *   _mainWindow;
	};

	inline ApplicationQt * const QT_APP() { return &Util::Generic::UniqueInstance<ApplicationQt>::get(); }

} // namespace VTX::UI::QT

#endif
