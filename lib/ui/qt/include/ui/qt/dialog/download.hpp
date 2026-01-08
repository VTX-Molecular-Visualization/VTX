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
	class Download : public BaseDialog<Download>
	{
	  public:
		Download();

	  private:
		QPointer<QComboBox>	   _comboBoxURL;
		QPointer<QComboBox>	   _comboBoxPDB;
		QPointer<QRadioButton> _radioButtonOpen;
		QPointer<QRadioButton> _radioButtonDownload;
		QString				   _url;
		QString				   _pdb;

		void _saveHistory( const QString & p_key, const QString & p_value );
		void _loadHistory( const QString & p_key, QComboBox * const p_comboBox );
	};

} // namespace VTX::UI::QT::Dialog

#endif
