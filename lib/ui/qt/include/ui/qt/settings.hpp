#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>

namespace VTX::UI::QT
{

	/**
	 * @brief Settings manager inheriting from QSettings.
	 */
	class Settings : public QSettings
	{
	  public:
		/**
		 * @brief Create settings from .ini file.
		 */
		Settings();

		/**
		 * @brief Save or restore all registered savable/restorable objects.
		 */
		void save();
	};

} // namespace VTX::UI::QT

#endif
