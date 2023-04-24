#ifndef __VTX_UI_QT_DIALOG__
#define __VTX_UI_QT_DIALOG__

#include <QString>
#include <exception>
#include <app/core/action/base_action.hpp>

namespace VTX
{
	// namespace IO::Struct
	//{
	//	class ImageExport;
	// }

	namespace UI::QT::Dialog
	{
		void openInformationDialog( const QString & p_title, const QString & p_message );
		void confirmActionDialog( VTX::App::Core::Action::BaseAction * const p_action,
								  const QString &				  p_title,
								  const QString &				  p_message );

		void exceptionDialog( const std::exception & e );
		void unhandledException();

		// static void openAdvancedSettingImageExportDialog();
		// static bool openExportImageDialog( const IO::Struct::ImageExport & p_exportData );

		// static void importRepresentationPresetDialog();
		// static void importRenderEffectPresetDialog();

		// static QString		_toQPath( const FilePath & p_path );

	} // namespace UI::QT::Dialog
} // namespace VTX

#endif
