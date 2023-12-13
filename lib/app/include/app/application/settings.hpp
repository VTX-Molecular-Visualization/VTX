#ifndef __VTX_APP_APPLICATION_SETTING__
#define __VTX_APP_APPLICATION_SETTING__

#include "app/component/render/enum_camera.hpp"
#include <any>
#include <concepts>
#include <string>
#include <util/concepts.hpp>
#include <util/variant.hpp>

namespace VTX::App::Application
{
	struct Settings
	{
	  private:
		struct UniquePointerStorage
		{
			template<typename T>
			void setValue( const T & p_value )
			{
				std::any any = p_value;
				_valuePtr	 = std::make_unique<std::any>( any );
			}
			template<typename T>
			void setDefaultValue( const T & p_defaultValue )
			{
				std::any any	 = p_defaultValue;
				_defaultValuePtr = std::make_unique<std::any>( any );
			}

			std::any * getValuePtr() { return _valuePtr.get(); }
			std::any * getDefaultValuePtr() { return _defaultValuePtr.get(); }

		  private:
			std::unique_ptr<std::any> _valuePtr		   = nullptr;
			std::unique_ptr<std::any> _defaultValuePtr = nullptr;
		};

	  public:
		template<Util::VariantValueConcept T>
		void referenceSetting( const std::string & p_key, T p_defaultValue = T() )
		{
			assert( !_settings.contains( p_key ) );

			_settings[ p_key ]		= p_defaultValue;
			_defaultValues[ p_key ] = p_defaultValue;
		}
		template<EnumConcept E>
		void referenceSetting( const std::string & p_key, E p_defaultValue = E() )
		{
			assert( !_settings.contains( p_key ) );

			_settings[ p_key ]		= int( p_defaultValue );
			_defaultValues[ p_key ] = int( p_defaultValue );
		}
		template<typename T>
		void referenceSetting( const std::string & p_key, const T & p_defaultValue = T() )
		{
			assert( !_settings.contains( p_key ) );

			_ptrStorages.emplace( p_key, UniquePointerStorage() );

			UniquePointerStorage & ptrStorage = _ptrStorages[ p_key ];
			ptrStorage.setDefaultValue( p_defaultValue );
			ptrStorage.setValue( p_defaultValue );

			_settings[ p_key ]		= ptrStorage.getValuePtr();
			_defaultValues[ p_key ] = ptrStorage.getDefaultValuePtr();
		}

		template<Util::VariantValueConcept T>
		const T get( const std::string p_key ) const
		{
			assert( _settings.contains( p_key ) );
			return _settings[ p_key ].get<T>();
		}
		template<EnumConcept E>
		const E get( const std::string p_key ) const
		{
			assert( _settings.contains( p_key ) );
			return E( _settings[ p_key ].get<int>() );
		}
		template<typename T>
		const T get( const std::string & p_key ) const
		{
			assert( _settings.contains( p_key ) );

			const std::any * const anyPtr = _settings[ p_key ].getPtr<std::any>();
			const T &			   value  = std::any_cast<T>( *anyPtr );

			return value;
		}

		template<Util::VariantValueConcept T>
		void set( const std::string p_key, const T & p_value )
		{
			assert( _settings.contains( p_key ) );
			_settings[ p_key ] = p_value;
		}
		template<EnumConcept E>
		void set( const std::string p_key, const E & p_value )
		{
			assert( _settings.contains( p_key ) );
			_settings[ p_key ] = int( p_value );
		}
		template<typename T>
		void set( const std::string & p_key, const T & p_value ) const
		{
			assert( _settings.contains( p_key ) );
			*( _settings[ p_key ].getPtr<std::any>() ) = p_value;
		}

		void reset();

		const Util::VariantMap & getMap() const { return _settings; }
		void					 setMap( const Util::VariantMap & p_mapSettings ) { _settings = p_mapSettings; }

	  private:
		// Mutable to allow bracket access in const functions (contains checked in asserts)
		mutable Util::VariantMap _settings = Util::VariantMap();

		Util::VariantMap _defaultValues = Util::VariantMap();

		std::map<std::string, UniquePointerStorage> _ptrStorages = std::map<std::string, UniquePointerStorage>();
	};

} // namespace VTX::App::Application
#endif
