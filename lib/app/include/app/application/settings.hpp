#ifndef __VTX_APP_APPLICATION_SETTINGS__
#define __VTX_APP_APPLICATION_SETTINGS__

#include "app/application/setting.hpp"
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <util/json/basic_json.hpp>

namespace VTX::App::Application
{
	class Settings
	{
	  public:
		using SettingMap = std::map<std::string, std::unique_ptr<BaseSetting>>;
		using JSonMap	 = std::map<std::string, Util::JSon::BasicJSon>;

	  public:
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

			if ( _settingJSons.contains( p_key ) )
			{
				try
				{
					_getSetting<T>( p_key ).deserialize( _settingJSons[ p_key ] );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "{}", e.what() );
					VTX_WARNING( "Unable to deserialize setting {}. Keep previous value.", p_key );
				}

				_settingJSons.erase( p_key );
			}

			return _getConstSetting<T>( p_key ).get();
		}

		template<typename T>
		void set( const std::string p_key, const T & p_value )
		{
			assert( _settings.contains( p_key ) );

			if ( _settingJSons.contains( p_key ) )
			{
				_settingJSons.erase( p_key );
			}

			_getSetting<T>( p_key ).set( p_value );
		}

		inline bool contains( const std::string & p_key ) const { return _settings.contains( p_key ); }

		inline const SettingMap & getSettingMap() const { return _settings; }
		inline const JSonMap &	  getJSonMap() const { return _settingJSons; }
		inline JSonMap &		  getJSonMap() { return _settingJSons; }

		void reset();

	  private:
		// Mutable to allow bracket access in const functions (contains checked in asserts)
		mutable SettingMap _settings	 = SettingMap();
		mutable JSonMap	   _settingJSons = JSonMap();

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
