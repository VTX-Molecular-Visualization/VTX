#ifndef __VTX_UI_DIALOG_DOWNLOAD_MOLECULE__
#define __VTX_UI_DIALOG_DOWNLOAD_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Dialog
			{
				class DownloadMoleculeDialog : public BaseManualWidget<QDialog>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void localize() override;
					void cancelAction();
					void openAction();

				  protected:
					DownloadMoleculeDialog( QWidget * p_parent );

					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QLineEdit *		   _fileLineEdit  = nullptr;
					QDialogButtonBox * _dialogButtons = nullptr;
				};
			} // namespace Dialog
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
