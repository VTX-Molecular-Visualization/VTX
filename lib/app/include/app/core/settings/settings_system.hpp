#ifndef __VTX_APP_CORE_SETTINGS_SYSTEM__
#define __VTX_APP_CORE_SETTINGS_SYSTEM__

#include "base_setting.hpp"
#include "setting_change_info.hpp"
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Settings
{
	// TODO: use Util::Collection.
	using SettingMap = std::map<std::string, std::unique_ptr<BaseSetting>>;

	class SettingsSystem
	{
	  public:
		SettingsSystem() = default;
		SettingsSystem( const SettingsSystem & p_source );

		template<typename T>
		void referenceSetting( const std::string & p_key, const T & p_defaultValue = T() )
		{
			assert( not _settings.contains( p_key ) );

			_settings[ p_key ] = std::make_unique<Setting<T>>( p_defaultValue, p_defaultValue );
		}

		template<typename T>
		const T & get( const std::string & p_key ) const
		{
			assert( _settings.contains( p_key ) );
			return _getConstSetting<T>( p_key ).get();
		}

		template<typename T>
		void set( const std::string & p_key, const T & p_value )
		{
			assert( _settings.contains( p_key ) );

			const T & previousValue = _getSetting<T>( p_key ).get();

			if ( previousValue != p_value )
			{
				_getSetting<T>( p_key ).set( p_value );
				onSetting( SettingChangeInfo<T>( p_key, previousValue, p_value ) );
			}
		}

		inline bool contains( const std::string & p_key ) const { return _settings.contains( p_key ); }

		inline const SettingMap & getSettingMap() const { return _settings; }

		void reset();

		friend bool operator==( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs );
		friend bool operator!=( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs );

		Util::Callback<BaseSettingChangeInfo> onSetting;

	  private:
		// Mutable to allow bracket access in const functions (contains checked in asserts)
		mutable SettingMap _settings = SettingMap();

		template<typename T>
		Setting<T> & _getSetting( const std::string & p_key ) const
		{
			return dynamic_cast<Setting<T> &>( *_settings[ p_key ] );
		}
		template<typename T>
		const Setting<T> & _getConstSetting( const std::string & p_key ) const
		{
			return dynamic_cast<const Setting<T> &>( *_settings[ p_key ] );
		}
	};

} // namespace VTX::App::Core::Settings

namespace VTX::App
{
	inline Core::Settings::SettingsSystem & SETTINGS_SYSTEM()
	{
		return Util::Singleton<Core::Settings::SettingsSystem>::get();
	}
} // namespace VTX::App

#endif
