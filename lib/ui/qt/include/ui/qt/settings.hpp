#ifndef VTX_UI_QT_SETTINGS_HPP
#define VTX_UI_QT_SETTINGS_HPP

#include <QSettings>

namespace VTX::UI::QT
{
	constexpr QUtf8StringView SETTING_KEY_GEOMETRY { "main/geometry" };
	constexpr QUtf8StringView SETTING_KEY_STATE { "main/state" };

	constexpr QUtf8StringView SETTING_KEY_GRANULARITY { "selection/granularity" };

	constexpr QUtf8StringView SETTING_KEY_URL { "dialog/download/history/url" };
	constexpr QUtf8StringView SETTING_KEY_PDB { "dialog/download/history/pdb" };
	constexpr QUtf8StringView SETTING_KEY_WIDTH { "dialog/export_image/width" };
	constexpr QUtf8StringView SETTING_KEY_HEIGHT { "dialog/export_image/height" };
	constexpr QUtf8StringView SETTING_KEY_FORMAT { "dialog/export_image/format" };
	constexpr QUtf8StringView SETTING_KEY_OPACITY { "dialog/export_image/background_opacity" };
	constexpr QUtf8StringView SETTING_KEY_EXPORT_FOLDER { "dialog/export_image/folder" };
	constexpr QUtf8StringView SETTING_KEY_OPEN_FOLDER { "dialog/open/last_folder" };

	constexpr QUtf8StringView SETTING_KEY_COLORS_HIDE_NON_USUAL { "colors/hide_non_common" };

	constexpr QUtf8StringView SETTING_KEY_KEYBOARD_LAYOUT { "option/keyboard_layout" };
	constexpr QUtf8StringView SETTING_KEY_SAVE_POWER { "option/save_power" };
	constexpr QUtf8StringView SETTING_KEY_VSYNC { "option/vsync" };
	constexpr QUtf8StringView SETTING_KEY_TREE_VIEW_MODE { "option/tree_view_mode" };

	constexpr QUtf8StringView SETTING_KEY_GRAPHICS_CONFIG_PRESETS_EXPANDED { "tree/presets/graphics_config/expanded" };
	constexpr QUtf8StringView SETTING_KEY_COLOR_LAYOUT_PRESETS_EXPANDED { "tree/presets/color_layout/expanded" };
	constexpr QUtf8StringView SETTING_KEY_REPRESENTATION_PRESETS_EXPANDED { "tree/presets/representation/expanded" };
	constexpr QUtf8StringView SETTING_KEY_SCRIPTS_EXPANDED { "tree/scripts/expanded" };

	/**
	 * @brief Which resid to use to generate the sequence viewer
	 */
	constexpr QUtf8StringView SETTING_KEY_SEQUENCE_VTX_RESID { "sequence/resid" };

	constexpr QUtf8StringView SETTING_KEY_THEME { "style/theme" };
	constexpr QUtf8StringView SETTING_KEY_FONT { "style/font" };

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
