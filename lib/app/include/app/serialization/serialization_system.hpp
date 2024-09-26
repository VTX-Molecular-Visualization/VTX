#ifndef __VTX_APP_SERIALIZATION_SERIALIZATION_SYSTEM__
#define __VTX_APP_SERIALIZATION_SERIALIZATION_SYSTEM__

#include "app/core/serialization/serialization_system.hpp"
#include "app_serializers.hpp"
#include "scene_serializers.hpp"
#include <util/json/io.hpp>

namespace VTX::App::Serialization
{
	template<typename T>
	concept SerializableByDefaultConcept = ( not Util::JSon::BasicJSonConcept<T> ) && requires( T & p_obj ) {
		{ App::Serialization::serialize( p_obj ) } -> std::convertible_to<Util::JSon::BasicJSon>;
	};

	template<typename T>
	concept DeserializableByDefaultConcept
		= ( not Util::JSon::BasicJSonConcept<T> ) && requires( const Util::JSon::BasicJSon & p_json, T & p_obj ) {
			  App::Serialization::deserialize( p_json, p_obj );
		  };

	class SerializationSystem : public Core::Serialization::SerializationSystem
	{
	  public:
		template<typename T>
		class Reader
		{
		  private:
		  public:
			Reader( const SerializationSystem & p_serializer, const FilePath & p_path, T * const p_target ) :
				_serializer( p_serializer ), _path( p_path ), _target( p_target )
			{
			}

			void read() const
			{
				Util::JSon::Document fullDoc = Util::JSon::IO::open( _path );

				if ( not fullDoc.json().contains( "VERSION" ) || not fullDoc.json().contains( "DATA" ) )
					throw IOException( "Ill-formed save file" );

				Core::Serialization::Version fileVersion;
				_serializer.deserialize( fullDoc.json()[ "VERSION" ], fileVersion );

				if ( fileVersion > Core::Serialization::Version::CURRENT )
					throw IOException( "Can not read file, version is newer than VTX" );

				VTX::Util::JSon::BasicJSon & dataJSon = fullDoc.json()[ "DATA" ];

				if ( fileVersion < Core::Serialization::Version::CURRENT )
					_serializer.upgrade( dataJSon, *_target, fileVersion );

				_serializer.deserialize( dataJSon, *_target );
			}

		  private:
			const SerializationSystem & _serializer;
			FilePath					_path;
			T *							_target;
		};

		template<typename T>
		class Writer
		{
		  public:
			Writer(
				const SerializationSystem &			 p_serializer,
				const FilePath &					 p_path,
				const T *							 p_obj,
				const Core::Serialization::Version & p_version = Core::Serialization::Version::CURRENT
			) : _serializer( p_serializer ), _path( p_path ), _obj( p_obj ), _version( p_version )
			{
			}

			void write() const
			{
				const VTX::Util::JSon::Document jsonDoc
					= { { "VERSION", _serializer.serialize( _version ) }, { "DATA", _serializer.serialize( *_obj ) } };

				Util::JSon::IO::write( _path, jsonDoc );
			}

		  private:
			const SerializationSystem &	 _serializer;
			FilePath					 _path;
			Core::Serialization::Version _version;

			const T * _obj;
		};

	  public:
		// Directly read a file at path p_path and deserialize it in p_obj object.
		template<typename T>
		void readObject( const FilePath & p_path, T & p_obj )
		{
			const auto serializedObject = Reader<T>( *this, p_path, &p_obj );

			serializedObject.read();
		}

		// Directly serialize an object p_obj and write the json file at p_path
		template<typename T>
		void writeObject( const FilePath & p_path, const T & p_obj )
		{
			const auto serializedObject = Writer<T>( *this, p_path, &p_obj );

			serializedObject.write();
		}

		template<SerializableByDefaultConcept T>
		Util::JSon::BasicJSon serialize( const T & p_obj ) const
		{
			return App::Serialization::serialize( p_obj );
		}

		template<typename T>
		Util::JSon::BasicJSon serialize( const T & p_obj ) const
		{
			if ( !_mapSerializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No serializer found for {}", typeid( T ).name() );

			return std::any_cast<const SerializeFunc<T> &>( _mapSerializeFunctions.at( typeid( T ) ) )( p_obj );
		}

		template<DeserializableByDefaultConcept T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj ) const
		{
			App::Serialization::deserialize( p_jsonObj, p_obj );
		}

		template<typename T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj ) const
		{
			if ( !_mapDeserializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No deserializer found for {}", typeid( T ).name() );

			std::any_cast<const DeserializeFunc<T> &>( _mapDeserializeFunctions.at( typeid( T
			) ) )( p_jsonObj.getObject(), p_obj );
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
	};
} // namespace VTX::App::Serialization

namespace VTX::App
{
	inline Serialization::SerializationSystem & SERIALIZATION_SYSTEM()
	{
		return Util::Singleton<Serialization::SerializationSystem>::get();
	}
} // namespace VTX::App

#endif
