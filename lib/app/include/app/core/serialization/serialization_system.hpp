#ifndef __VTX_APP_CORE_SERIALIZATION_SERIALIZATION_SYSTEM__
#define __VTX_APP_CORE_SERIALIZATION_SERIALIZATION_SYSTEM__

#include "app/core/settings/settings_system.hpp"
#include "upgrade_stack.hpp"
#include "version.hpp"
#include <any>
#include <functional>
#include <map>
#include <typeindex>
#include <util/collection.hpp>
#include <util/concepts.hpp>
#include <util/exceptions.hpp>
#include <util/json/io.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Serialization
{
	class SerializationSystem
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

		/*
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
		*/

		/*
		template<typename T>
		using SerializeFunc = std::function<Util::JSon::Object( const T & )>;

		template<typename T>
		using DeserializeFunc = std::function<void( const Util::JSon::Object &, T & )>;
		*/

		/*
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
		*/

		/*
		template<typename T>
		void registerUpgrade( const Version & p_version, UpgradeFunc<T> p_upgradeFunc )
		{
			if ( not _mapUpgradeFunctions.contains( typeid( T ) ) )
				_mapUpgradeFunctions.emplace( typeid( T ), UpgradeStack<T>() );

			std::any &		  any	= _mapUpgradeFunctions[ typeid( T ) ];
			UpgradeStack<T> & stack = std::any_cast<UpgradeStack<T> &>( any );
			stack.add( p_version, p_upgradeFunc );
		}

		template<typename T>
		void upgrade( Util::JSon::BasicJSon & p_jsonObj, T & p_obj, const Version & p_version )
		{
			if ( _mapUpgradeFunctions.contains( typeid( T ) ) )
			{
				const std::any &		any	  = _mapUpgradeFunctions.at( typeid( T ) );
				const UpgradeStack<T> & stack = std::any_cast<const UpgradeStack<T> &>( any );
				stack.applyUpgrades( p_jsonObj, p_obj, p_version );
			}
		}
		*/

		/*
		template<Util::JSon::BasicJSonConcept T>
		bool canSerialize()
		{
			return true;
		}
		*/

		/*
		// Version
		Util::JSon::Object serialize( const App::Core::Serialization::Version & );
		void			   deserialize( const Util::JSon::Object &, App::Core::Serialization::Version & );

		// VTXApp
		Util::JSon::Object serialize( const VTXApp & );
		void			   deserialize( const Util::JSon::Object &, VTXApp & );

		// Settings
		template<typename T>
		void deserializeSetting( const Util::JSon::Object & p_json, T & p_setting )
		{
			p_setting.deserialize( p_json );
		}

		Util::JSon::Object serialize( const App::Core::Settings::SettingMap & );
		void			   deserialize( const Util::JSon::Object &, App::Core::Settings::SettingMap & );

		// Scene
		Util::JSon::Object serialize( const App::Application::Scene & );
		void			   deserialize( const Util::JSon::Object &, App::Application::Scene & );

		// SceneItemComponent
		Util::JSon::Object serialize( const Component::Scene::SceneItemComponent & p_component );
		void deserialize( const Util::JSon::Object & p_json, Component::Scene::SceneItemComponent & p_component );

		// TransformComponent
		Util::JSon::Object serialize( const Component::Scene::Transform & p_component );
		void			   deserialize( const Util::JSon::Object & p_json, Component::Scene::Transform & p_component );

		// MoleculeComponent
		Util::JSon::Object serialize( const Component::Chemistry::Molecule & p_component );
		void deserialize( const Util::JSon::Object & p_json, Component::Chemistry::Molecule & p_component );

		// MoleculeMetadata
		Util::JSon::Object serialize( const Component::IO::MoleculeMetadata & p_component );
		void deserialize( const Util::JSon::Object & p_json, Component::IO::MoleculeMetadata & p_component );
		*/

		// Customs.
		/*		template<typename T>
		Util::JSon::BasicJSon serialize( const T & p_obj )
		{
			if ( not _mapSerializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No serializer found for {}", typeid( T ).name() );

			return std::any_cast<const SerializeFunc<T> &>( _mapSerializeFunctions.at( typeid( T ) ) )( p_obj );
		}

		template<typename T>
		void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj )
		{
			if ( not _mapDeserializeFunctions.contains( typeid( T ) ) )
				throw VTXException( "No deserializer found for {}", typeid( T ).name() );

			std::any_cast<const DeserializeFunc<T> &>( _mapDeserializeFunctions.at( typeid( T
			) ) )( p_jsonObj.getObject(), p_obj );
		}

		template<typename T>
		bool canSerialize()
		{
			return _mapSerializeFunctions.contains( typeid( T ) );
		}
		*/

	  private:
		Util::Collection<Util::Generic::BaseSerializable *> _serialisables;
		/*
		std::map<std::type_index, std::any> _mapSerializeFunctions;
		std::map<std::type_index, std::any> _mapDeserializeFunctions;
		std::map<std::type_index, std::any> _mapUpgradeFunctions;
		*/
	};
} // namespace VTX::App::Core::Serialization

namespace VTX::App
{
	inline Core::Serialization::SerializationSystem & SERIALIZATION_SYSTEM()
	{
		return Util::Singleton<Core::Serialization::SerializationSystem>::get();
	}
} // namespace VTX::App

#endif
