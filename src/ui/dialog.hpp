#ifndef __VTX_UI_DIALOG__
#define __VTX_UI_DIALOG__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/base_action.hpp"
#include <QString>
#include <QWidget>

namespace VTX::UI
{
	class Dialog
	{
	  public:
		static void openDownloadMoleculeDialog();
		static void openDownloadMoleculeDialog( const QString & p_pdbCode );

		static void openLoadMoleculeDialog();
		static void openExportMoleculeDialog();

		static void openLoadSessionDialog();
		static void openSaveSessionDialog();

		static void importRepresentationPresetDialog();
		static void importRenderEffectPresetDialog();

		static void confirmActionDialog( QWidget * const			p_caller,
										 Action::BaseAction * const p_action,
										 const QString &			p_title,
										 const QString &			p_message );
	};

} // namespace VTX::UI

#endif
