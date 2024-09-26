#ifndef __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__
#define __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__

#include "upgrade_stack.hpp"
#include "util_serializers.hpp"
#include "version.hpp"
#include <any>
#include <functional>
#include <map>
#include <typeindex>
#include <util/exceptions.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>

namespace VTX::App::Core::Serialization
{

	template<Util::JSon::BasicJSonConcept T>
	Util::JSon::BasicJSon serialize( const T & p_obj )
	{
		return Util::JSon::BasicJSon( p_obj );
	}

	template<Util::JSon::BasicJSonConcept T>
	void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj )
	{
		p_obj = p_jsonObj.get<T>();
	}

	template<Util::JSon::BasicJSonConcept T>
	bool canSerialize()
	{
		return true;
	}

	class SerializationSystem
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
		void upgrade( Util::JSon::BasicJSon & p_jsonObj, T & p_obj, const Version & p_version ) const
		{
			if ( _mapUpgradeFunctions.contains( typeid( T ) ) )
			{
				const std::any &		any	  = _mapUpgradeFunctions.at( typeid( T ) );
				const UpgradeStack<T> & stack = std::any_cast<const UpgradeStack<T> &>( any );
				stack.applyUpgrades( p_jsonObj, p_obj, p_version );
			}
		}

	  protected:
		std::map<std::type_index, std::any> _mapSerializeFunctions;
		std::map<std::type_index, std::any> _mapDeserializeFunctions;
		std::map<std::type_index, std::any> _mapUpgradeFunctions;
	};
} // namespace VTX::App::Core::Serialization

#endif
