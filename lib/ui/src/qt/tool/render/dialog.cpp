#include "ui/qt/tool/render/dialog.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include <QMessageBox>
#include <string>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Render::Dialog
{
	void Dialog::openGLInitializationFail()
	{
		const std::string msg
			= "Unable to create OpenGL 4.5 context. Update your drivers and check your hardware compatibility.";
		VTX_ERROR( "{}", msg );

		QMessageBox::critical( &QT_APP()->getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );

		std::exit( EXIT_FAILURE );
	}
} // namespace VTX::UI::QT::Tool::Render::Dialog
