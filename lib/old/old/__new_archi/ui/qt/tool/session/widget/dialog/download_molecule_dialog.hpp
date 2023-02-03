#ifndef __VTX_UI_QT_TOOL_SESSION_WIDGET_DOWNLOAD_MOLECULE_DIALOG__
#define __VTX_UI_QT_TOOL_SESSION_WIDGET_DOWNLOAD_MOLECULE_DIALOG__

#include "__new_archi/ui/qt/widget/base_dialog.hpp"
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QShowEvent>
#include <QString>

namespace VTX::UI::QT::Tool::Session::Widget::Dialog
{
	class DownloadMoleculeDialog : public QT::Widget::BaseDialog
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		static DownloadMoleculeDialog * pop();
		static DownloadMoleculeDialog * pop( const QString & p_id );

		void localize() override;
		void cancelAction();
		void openAction();

		void setPdbId( const QString & p_id );

	  protected:
		DownloadMoleculeDialog( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		void _refreshComboBoxList();

		QComboBox *		   _fileComboBox  = nullptr;
		QDialogButtonBox * _dialogButtons = nullptr;
	};
} // namespace VTX::UI::QT::Tool::Session::Widget::Dialog

#endif
