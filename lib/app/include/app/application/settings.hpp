#ifndef __VTX_APP_APPLICATION_SETTINGS__
#define __VTX_APP_APPLICATION_SETTINGS__

#include "app/application/setting.hpp"
#include <app/core/callback_event.hpp>
#include <cassert>
#include <map>
#include <memory>
#include <string>

namespace VTX::App::Application
{
	class BaseSettingChangeEvent
	{
	  public:
		BaseSettingChangeEvent( const std::string & p_key ) : key( p_key ) {};
		const std::string key;
	};

	template<typename T>
	class SettingChangeEvent : public BaseSettingChangeEvent
	{
	  public:
		SettingChangeEvent( const std::string & p_key, const T & p_oldValue, const T & p_newValue ) :
			BaseSettingChangeEvent( p_key ), oldValue( p_oldValue ), newValue( p_newValue ) {};

		const T & oldValue;
		const T & newValue;
	};

	class Settings
	{
	  public:
		using SettingMap = std::map<std::string, std::unique_ptr<BaseSetting>>;

	  public:
		Settings() = default;
		Settings( const Settings & p_source );

		template<typename T>
		void referenceSetting( const std::string & p_key, const T & p_defaultValue = T() )
		{
			assert( !_settings.contains( p_key ) );

			_settings[ p_key ] = std::make_unique<Setting<T>>( p_defaultValue, p_defaultValue );
		}

		template<typename T>
		const T & get( const std::string p_key ) const
		{
			assert( _settings.contains( p_key ) );
			return _getConstSetting<T>( p_key ).get();
		}

		template<typename T>
		void set( const std::string p_key, const T & p_value )
		{
			assert( _settings.contains( p_key ) );

			const T & previousValue = _getSetting<T>( p_key ).get();

			if ( previousValue != p_value )
			{
				_getSetting<T>( p_key ).set( p_value );

				const std::unique_ptr<SettingChangeEvent<T>> eventData
					= std::make_unique<SettingChangeEvent<T>>( p_key, previousValue, p_value );

				onSettingChange.call( eventData.get() );
			}
		}

		inline bool contains( const std::string & p_key ) const { return _settings.contains( p_key ); }

		inline const SettingMap & getSettingMap() const { return _settings; }

		void reset();

		friend bool operator==( const Settings & p_lhs, const Settings & p_rhs );
		friend bool operator!=( const Settings & p_lhs, const Settings & p_rhs );

		Core::CallbackEmitter<BaseSettingChangeEvent *> onSettingChange;

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

} // namespace VTX::App::Application
#endif
