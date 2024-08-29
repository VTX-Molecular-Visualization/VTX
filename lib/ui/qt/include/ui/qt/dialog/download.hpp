#ifndef __VTX_UI_QT_DIALOG_DOWNLOAD__
#define __VTX_UI_QT_DIALOG_DOWNLOAD__

#include "ui/qt/base_widget.hpp"
#include <QComboBox>
#include <QDialog>
#include <util/string.hpp>

namespace VTX::UI::QT::Dialog
{

	class Download : public BaseWidget<Download, QDialog>
	{
	  public:
		Download();
		virtual ~Download() {}

	  private:
		// TODO: move to json.
		inline static const std::string _PDB_ID_TEMPLATE = "[PDB_ID]";
		inline static const std::string _DEFAULT_URL = "https://files.rcsb.org/download/" + _PDB_ID_TEMPLATE + ".pdb ";
		inline static const std::string _SETTING_KEY_URL = "dialogDownloadURLHistory";
		inline static const std::string _SETTING_KEY_PDB = "dialogDownloadPDBHistory";
		// TODO: move to settings.
		inline static const uint _MAX_HISTORY_SIZE = 10;

		void _loadHistory( const std::string & p_key, QComboBox * const p_comboBox );
		void _saveHistory( const std::string & p_key, const std::string & p_value );
	};

} // namespace VTX::UI::QT::Dialog

#endif
