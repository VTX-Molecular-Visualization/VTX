#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>

namespace VTX::UI::QT
{
	const QString SETTING_KEY_GEOMETRY = "main/geometry";
	const QString SETTING_KEY_STATE	   = "main/state";

	const QString SETTING_KEY_GRANULARITY = "selection/granularity";

	const QString SETTING_KEY_URL			= "dialog/download/history/url";
	const QString SETTING_KEY_PDB			= "dialog/download/history/pdb";
	const QString SETTING_KEY_WIDTH			= "dialog/export_image/width";
	const QString SETTING_KEY_HEIGHT		= "dialog/export_image/height";
	const QString SETTING_KEY_FORMAT		= "dialog/export_image/format";
	const QString SETTING_KEY_OPACITY		= "dialog/export_image/background_opacity";
	const QString SETTING_KEY_EXPORT_FOLDER = "dialog/export_image/folder";
	const QString SETTING_KEY_OPEN_FOLDER	= "dialog/open/last_folder";

	const QString SETTING_KEY_COLORS_HIDE_NON_USUAL = "colors/hide_non_common";

	const QString SETTING_KEY_KEYBOARD_LAYOUT = "option/keyboard_layout";
	const QString SETTING_KEY_SAVE_POWER	  = "option/save_power";
	const QString SETTING_KEY_VSYNC			  = "option/vsync";
	const QString SETTING_KEY_TREE_VIEW_MODE  = "option/tree_view_mode";

	const QString SETTING_KEY_GRAPHICS_CONFIG_PRESETS_EXPANDED = "tree/presets/graphics_config/expanded";
	const QString SETTING_KEY_COLOR_LAYOUT_PRESETS_EXPANDED	   = "tree/presets/color_layout/expanded";
	const QString SETTING_KEY_REPRESENTATION_PRESETS_EXPANDED  = "tree/presets/representation/expanded";
	const QString SETTING_KEY_SCRIPTS_EXPANDED				   = "tree/scripts/expanded";

	/**
	 * @brief Which resid to use to generate the sequence viewer
	 */
	const QString SETTING_KEY_SEQUENCE_VTX_RESID = "sequence/resid";

	const QString SETTING_KEY_THEME = "style/theme";
	const QString SETTING_KEY_FONT	= "style/font";

	/**
	 * @brief Settings manager inheriting from QSettings.
	 */
	class Settings : public QSettings
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Create settings from .ini file.
		 */
		Settings();

		/**
		 * @brief Save or restore all registered savable/restorable objects.
		 */
		void save();

		/*
		void setValue( QAnyStringView p_key, const QVariant & p_value )
		{
			QSettings::setValue( p_key, p_value );
			emit settingChanged( p_key, p_value );
		}

		QVariant value( QAnyStringView p_key, const QVariant & p_defaultValue ) const
		{
			return QSettings::value( p_key, p_defaultValue );
		}

		QVariant value( QAnyStringView p_key ) const { return QSettings::value( p_key ); }

	  signals:
		void settingChanged( QAnyStringView, const QVariant & );
		*/
	};

} // namespace VTX::UI::QT

#endif
