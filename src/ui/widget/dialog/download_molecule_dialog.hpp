#ifndef __VTX_UI_DIALOG_DOWNLOAD_MOLECULE__
#define __VTX_UI_DIALOG_DOWNLOAD_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace VTX::UI::Widget::Dialog
{
	class DownloadMoleculeDialog : public BaseManualWidget<QDialog>
	{
		VTX_WIDGET

	  public:
		static void openDialog();
		static void openDialog( const QString & p_txt );

		void localize() override;
		void cancelAction();
		void openAction();

	  protected:
		DownloadMoleculeDialog( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		inline static DownloadMoleculeDialog * _instance = nullptr;
		static DownloadMoleculeDialog &		   _getInstance();

		QLineEdit *		   _fileLineEdit  = nullptr;
		QDialogButtonBox * _dialogButtons = nullptr;
	};
} // namespace VTX::UI::Widget::Dialog

#endif
