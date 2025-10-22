#ifndef __VTX_UI_QT_DIALOG_DOWNLOAD__
#define __VTX_UI_QT_DIALOG_DOWNLOAD__

#include "ui/qt/dialog/base_dialog.hpp"
#include "ui/qt/settings.hpp"
#include <QComboBox>
#include <QPointer>
#include <QRadioButton>
#include <util/string.hpp>
#include <util/url.hpp>

namespace VTX::UI::QT::Dialog
{
	class Download : public BaseDialog<>
	{
	  public:
		Download();

		void save( Settings & ) override;
		void restore( const Settings & ) override;

	  private:
		// TODO: check if file exists in cache.

		// TODO: move to json.
		inline static const QString _PDB_ID_TEMPLATE = VTX::Util::Url::systemReplacementToken();
		inline static const QString _DEFAULT_URL
			= QString( VTX::Util::Url::rcsbPdbDownloadBaseUrl() ) + _PDB_ID_TEMPLATE + ".pdb ";
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

		void _saveHistory( Settings &, const QString & p_key, const QString & p_value );
		void _loadHistory( const Settings &, const QString & p_key, QComboBox * const p_comboBox );
	};

} // namespace VTX::UI::QT::Dialog

#endif
