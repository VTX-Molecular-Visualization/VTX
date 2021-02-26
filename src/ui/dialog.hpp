#ifndef __VTX_UI_DIALOG__
#define __VTX_UI_DIALOG__

#ifdef _MSC_VER
#pragma once
#endif

#include <QString>
#include <QWidget>

namespace VTX::UI
{
	class Dialog
	{
	  public:
		static void openDownloadMoleculeDialog();
		static void openDownloadMoleculeDialog( const QString & p_pdbCode );

		static void openLoadMoleculeDialog( QWidget * const p_caller );
	};

} // namespace VTX::UI

#endif
