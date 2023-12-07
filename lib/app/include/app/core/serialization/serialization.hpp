#ifndef __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__
#define __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__

#include "app/core/serialization/upgrade_stack.hpp"
#include "app/core/serialization/version.hpp"
#include "app/internal/serialization/all_serializers.hpp"
#include <any>
#include <functional>
#include <map>
#include <typeindex>
#include <util/json/json.hpp>

namespace VTX::App::Core::Serialization
{
	template<typename T>
	concept SerializableByDefaultConcept
		= (!Util::JSon::BasicJSonConcept<T>) && requires( T p_obj ) {
													{
														Internal::Serialization::serialize( p_obj )
														} -> std::convertible_to<Util::JSon::BasicJSon>;
												};
	template<typename T>
	concept DeserializableByDefaultConcept = requires( const Util::JSon::BasicJSon & p_json, T & p_obj ) {
												 Internal::Serialization::deserialize( p_json, p_obj );
											 };

	class Serialization
	{
	  public:
		template<typename T>
		using SerializeFunc = std::function<Util::JSon::Object( const T & )>;

		template<typename T>
		using DeserializeFunc = std::function<void( const Util::JSon::Object &, T & )>;

		template<typename T>
		void registerSerializationFunction( SerializeFunc<T> p_serializationFunction )
		{
			_mapSerializeFunctions.emplace( typeid( T ), p_serializationFunction );
		}
		template<typename T>
		void registerDeserializationFunction( DeserializeFunc<T> p_deserializationFunction )
		{
			_mapDeserializeFunctions.emplace( typeid( T ), p_deserializationFunction );
		}

		template<Util::JSon::BasicJSonConcept T>
		Util::JSon::BasicJSon serialize( const T & p_obj ) const
		{
			return Util::JSon::BasicJSon( p_obj );
		}
		template<SerializableByDefaultConcept T>
		Util::JSon::BasicJSon serialize( const T & p_obj ) const
		{
			return App::Internal::Serialization::serialize( p_obj );
		}
		template<typename T>
		Util::JSon::BasicJSon serialize( const T & p_obj ) const
		{
			if ( !_mapSerializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No serializer found for " + std::string( typeid( T ).name() ) );

			return std::any_cast<const SerializeFunc<T> &>( _mapSerializeFunctions.at( typeid( T ) ) )( p_obj );
		}

		template<Util::JSon::BasicJSonConcept T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj ) const
		{
			p_obj = p_jsonObj.get<T>();
		}
		template<DeserializableByDefaultConcept T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj ) const
		{
			App::Internal::Serialization::deserialize( p_jsonObj, p_obj );
		}
		template<typename T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj ) const
		{
			if ( !_mapDeserializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No deserializer found for " + std::string( typeid( T ).name() ) );

			std::any_cast<const DeserializeFunc<T> &>( _mapDeserializeFunctions.at( typeid( T ) )
			)( p_jsonObj.getObject(), p_obj );
		}

		template<typename T>
		T deserializeField(
			const Util::JSon::Object & p_jsonObj,
			const std::string &		   p_key,
			const T &				   p_defaultValue = T()
		)
		{
			T res;

			if ( p_jsonObj.contains( p_key ) )
			{
				try
				{
					deserialize( p_jsonObj[ p_key ], res );
				}
				catch ( const std::exception & e )
				{
					VTX_WARNING( "Issue during deserialization of {} ({}). Default value assigned.", p_key, e.what() );
					res = p_defaultValue;
				}
			}
			else
			{
				res = p_defaultValue;
			}

			return res;
		}

		template<Util::JSon::BasicJSonConcept T>
		bool canSerialize() const
		{
			return true;
		}
		template<SerializableByDefaultConcept T>
		bool canSerialize() const
		{
			return true;
		}
		template<typename T>
		bool canSerialize() const
		{
			return _mapSerializeFunctions.contains( typeid( T ) );
		}

		template<typename T>
		void registerUpgrade( const Version & p_version, UpgradeFunc<T> p_upgradeFunc )
		{
			if ( !_mapUpgradeFunctions.contains( typeid( T ) ) )
				_mapUpgradeFunctions.emplace( typeid( T ), UpgradeStack<T>() );

			std::any &		  any	= _mapUpgradeFunctions[ typeid( T ) ];
			UpgradeStack<T> & stack = std::any_cast<UpgradeStack<T> &>( any );
			stack.add( p_version, p_upgradeFunc );
		}

		template<typename T>
		void migrate( Util::JSon::BasicJSon & p_jsonObj, T & p_obj, const Version & p_version ) const
		{
			const std::any &		any	  = _mapUpgradeFunctions.at( typeid( T ) );
			const UpgradeStack<T> & stack = std::any_cast<const UpgradeStack<T> &>( any );
			stack.applyUpgrades( p_jsonObj, p_obj, p_version );
		}

	  private:
		std::map<std::type_index, std::any> _mapSerializeFunctions	 = std::map<std::type_index, std::any>();
		std::map<std::type_index, std::any> _mapDeserializeFunctions = std::map<std::type_index, std::any>();

		std::map<std::type_index, std::any> _mapUpgradeFunctions = std::map<std::type_index, std::any>();
	};
} // namespace VTX::App::Core::Serialization

#endif
