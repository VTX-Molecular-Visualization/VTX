#include "dialog.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include <old/tool/logger.hpp>
#include <QMessageBox>
#include <string>

namespace VTX::UI::QT::Tool::Render::Dialog
{
	void Dialog::openGLInitializationFail()
	{
		std::string msg
			= "Unable to create OpenGL 4.5 context. Update your drivers and check your hardware compatibility.";
		VTX_ERROR( msg );

		QMessageBox::critical( &QT_APP()->getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );

		std::exit( EXIT_FAILURE );
	}
} // namespace VTX::UI::QT::Tool::Render::Dialog
