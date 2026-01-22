#ifndef __VTX_UI_QT_SETTINGS__
#define __VTX_UI_QT_SETTINGS__

#include <QSettings>

namespace VTX::UI::QT
{
	constexpr std::string_view _SETTING_KEY_GRANULARITY = "inspector/picking_granularity";

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
