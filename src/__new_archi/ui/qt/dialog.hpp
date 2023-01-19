#ifndef __VTX_UI_QT_DIALOG__
#define __VTX_UI_QT_DIALOG__

#include "action/base_action.hpp"
#include "worker/worker_manager.hpp"
#include <QString>
#include <QWidget>
#include <exception>

namespace VTX
{
	// namespace IO::Struct
	//{
	//	class ImageExport;
	// }

	namespace UI::QT::Dialog
	{
		void openInformationDialog( const QString & p_title, const QString & p_message );
		void confirmActionDialog( VTX::Action::BaseAction * const p_action,
								  const QString &				  p_title,
								  const QString &				  p_message );

		void openGLInitializationFail();
		void exceptionDialog( const std::exception & e );
		void unhandledException();

		// static void openAdvancedSettingImageExportDialog();
		// static bool openExportImageDialog( const IO::Struct::ImageExport & p_exportData );

		// static void importRepresentationPresetDialog();
		// static void importRenderEffectPresetDialog();

		// static QString		_toQPath( const Util::FilePath & p_path );

	} // namespace UI::QT::Dialog
} // namespace VTX

#endif
