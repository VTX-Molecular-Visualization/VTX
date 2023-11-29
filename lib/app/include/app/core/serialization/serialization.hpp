#ifndef __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__
#define __VTX_APP_CORE_SERIALIZATION_SERIALIZATION__

#include "app/internal/serialization/all_serializers.hpp"
#include <any>
#include <functional>
#include <map>
#include <typeindex>
#include <util/json/json.hpp>

namespace VTX::App::Core::Serialization
{
	template<typename T>
	concept SerializableByDefaultConcept = requires( T p_obj ) {
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
			assert( _mapSerializeFunctions.contains( typeid( T ) ) );

			return std::any_cast<SerializeFunc<T>>( _mapSerializeFunctions.at( typeid( T ) ) )( p_obj );
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
			assert( _mapDeserializeFunctions.contains( typeid( T ) ) );

			std::any_cast<DeserializeFunc<T>>( _mapDeserializeFunctions.at( typeid( T ) )
			)( p_jsonObj.getObject(), p_obj );
		}

		template<typename T>
		T deserializeFieldWithCheck(
			const Util::JSon::Object & p_jsonObj,
			const std::string &		   p_key,
			const T &				   p_defaultValue = T()
		)
		{
			T res;

			if ( p_jsonObj.contains( p_key ) )
				deserialize( p_jsonObj[ p_key ], res );
			else
				res = p_defaultValue;

			return res;
		}

		VTX::Util::JSon::Document readFile( const FilePath p_path ) const;
		void					  writeFile( const FilePath p_path, const VTX::Util::JSon::Document & p_doc ) const;

	  private:
		std::map<std::type_index, std::any> _mapSerializeFunctions	 = std::map<std::type_index, std::any>();
		std::map<std::type_index, std::any> _mapDeserializeFunctions = std::map<std::type_index, std::any>();
	};
} // namespace VTX::App::Core::Serialization

#endif
