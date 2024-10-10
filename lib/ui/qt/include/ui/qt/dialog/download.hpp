#ifndef __VTX_UI_QT_DIALOG_DOWNLOAD__
#define __VTX_UI_QT_DIALOG_DOWNLOAD__

#include "ui/qt/base_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QComboBox>
#include <QDialog>
#include <QPointer>
#include <QRadioButton>
#include <util/string.hpp>

namespace VTX::UI::QT::Dialog
{

	class Download : public BaseWidget<Download, QDialog>, public Savable
	{
	  public:
		Download();

		void save() override;
		void restore() override;

	  private:
		// TODO: check if file exists in cache.

		// TODO: move to json.
		inline static const QString _PDB_ID_TEMPLATE = "[PDB_ID]";
		inline static const QString _DEFAULT_URL	 = "https://files.rcsb.org/download/" + _PDB_ID_TEMPLATE + ".pdb ";
		inline static const QString _SETTING_KEY_URL = "dialog/download/history/url";
		inline static const QString _SETTING_KEY_PDB = "dialog/download/history/pdb";
		// TODO: move to settings.
		inline static const uint _MAX_HISTORY_SIZE = 10;

		QPointer<QComboBox>	   _comboBoxURL;
		QPointer<QComboBox>	   _comboBoxPDB;
		QPointer<QRadioButton> _radioButtonOpen;
		QPointer<QRadioButton> _radioButtonDownload;
		QString				   _url;
		QString				   _pdb;

		void _loadHistory( const QString & p_key, QComboBox * const p_comboBox );
		void _saveHistory( const QString & p_key, const QString & p_value );
	};

} // namespace VTX::UI::QT::Dialog

#endif
