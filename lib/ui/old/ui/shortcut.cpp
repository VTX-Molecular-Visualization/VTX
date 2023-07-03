#include "ui/old_ui/ui/shortcut.hpp"
#include "ui/old_ui/controller/base_keyboard_controller.hpp"
#include <QWidget>

namespace VTX::UI
{
	const QKeySequence Shortcut::Main::NEW_SCENE		 = QKeySequence( QWidget::tr( "Ctrl+N" ) );
	const QKeySequence Shortcut::Main::OPEN_SCENE		 = QKeySequence( QWidget::tr( "Ctrl+O" ) );
	const QKeySequence Shortcut::Main::DOWNLOAD_MOLECULE = QKeySequence( QWidget::tr( "Ctrl+Shift+O" ) );
	const QKeySequence Shortcut::Main::SAVE_SCENE		 = QKeySequence( QWidget::tr( "Ctrl+S" ) );
	const QKeySequence Shortcut::Main::SAVE_AS_SCENE	 = QKeySequence( QWidget::tr( "Ctrl+Shift+S" ) );

	const QKeySequence Shortcut::Main::FULLSCREEN	  = QKeySequence( QWidget::tr( "F11" ) );
	const QKeySequence Shortcut::Main::RESTORE_LAYOUT = QKeySequence( QWidget::tr( "F6" ) );

	// const QKeySequence Shortcut::Main::SELECT_ALL	   = QKeySequence( QWidget::tr(
	//	 Controller::BaseKeyboardController::getKeyboardLayout() == Controller::KeyboardLayout::AZERTY ? "Ctrl+A"
	//																								   : "Ctrl+Q" ) );
	const QKeySequence Shortcut::Main::SELECT_ALL	   = QKeySequence( QWidget::tr( "Ctrl+A" ) );
	const QKeySequence Shortcut::Main::CLEAR_SELECTION = QKeySequence( QWidget::tr( "Esc" ) );

	const QKeySequence Shortcut::Main::DUPLICATE_SELECTION = QKeySequence( QWidget::tr( "Ctrl+D" ) );
	const QKeySequence Shortcut::Main::EXTRACT_SELECTION   = QKeySequence( QWidget::tr( "Ctrl+E" ) );
	const QKeySequence Shortcut::Main::DELETE_SELECTION	   = QKeySequence( QWidget::tr( "Del" ) );
	const QKeySequence Shortcut::Main::ORIENT_ON_SELECTION = QKeySequence( QWidget::tr( "O" ) );

	const QKeySequence Shortcut::Main::SET_SELECTION_PICKER	  = QKeySequence( QWidget::tr( "P" ) );
	const QKeySequence Shortcut::Main::SET_MEASUREMENT_PICKER = QKeySequence( QWidget::tr( "M" ) );

	const QKeySequence Shortcut::Render::TOGGLE_CAMERA_CONTROLLER = QKeySequence( QWidget::tr( "F1" ) );
	const QKeySequence Shortcut::Render::RESET_CAMERA_CONTROLLER  = QKeySequence( QWidget::tr( "Ctrl+F1" ) );
	const QKeySequence Shortcut::Render::TOGGLE_CAMERA			  = QKeySequence( QWidget::tr( "F2" ) );
	const QKeySequence Shortcut::Render::ADD_VIEWPOINT			  = QKeySequence( QWidget::tr( "F3" ) );
	const QKeySequence Shortcut::Render::SNAPSHOT				  = QKeySequence( QWidget::tr( "F5" ) );
	const QKeySequence Shortcut::Render::TOGGLE_ALL_OVERLAYS	  = QKeySequence( QWidget::tr( "F4" ) );

	const QKeySequence Shortcut::Dev::COMPILE_SHADER	 = QKeySequence( QWidget::tr( "F8" ) );
	const QKeySequence Shortcut::Dev::ACTIVE_RENDERER	 = QKeySequence( QWidget::tr( "F9" ) );
	const QKeySequence Shortcut::Dev::REFRESH_SES		 = QKeySequence( QWidget::tr( "F10" ) );
	const QKeySequence Shortcut::Dev::CHANGE_RENDER_MODE = QKeySequence( QWidget::tr( "F7" ) );
	const QKeySequence Shortcut::Dev::PRINT_CAMERA_INFOS = QKeySequence( QWidget::tr( "Space" ) );
} // namespace VTX::UI
