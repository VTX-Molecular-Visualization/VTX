#ifndef __VTX_UI_QT_DIALOG_DOWNLOAD_MOLECULE__
#define __VTX_UI_QT_DIALOG_DOWNLOAD_MOLECULE__

#include "__new_archi/ui/qt/widget/base_manual_widget.hpp"
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QShowEvent>

namespace VTX::UI::QT::Widget::Dialog
{
	class DownloadMoleculeDialog : public BaseManualWidget<QDialog>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		static void openDialog();
		static void openDialog( const QString & p_txt );

		void localize() override;
		void cancelAction();
		void openAction();

	  protected:
		DownloadMoleculeDialog( QWidget * p_parent );

		void showEvent( QShowEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		inline static DownloadMoleculeDialog * _instance = nullptr;
		static DownloadMoleculeDialog &		   _getInstance();

		void _refreshComboBoxList();

		QComboBox *		   _fileComboBox  = nullptr;
		QDialogButtonBox * _dialogButtons = nullptr;
	};
} // namespace VTX::UI::QT::Widget::Dialog

#endif
