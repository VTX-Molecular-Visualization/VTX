#ifndef __VTX_APP_APPLICATION_SYSTEM_SERIALIZER__
#define __VTX_APP_APPLICATION_SYSTEM_SERIALIZER__

#include "app/core/io/reader/serialized_object.hpp"
#include "app/core/io/writer/serialized_object.hpp"
#include "app/core/serialization/serialization.hpp"

namespace VTX::App::Application::System
{
	class Serializer final : public Core::Serialization::Serialization
	{
	  public:
		// Directly read a file at path p_path and deserialize it in p_obj object.
		template<typename T>
		void readObject( const FilePath & p_path, T & p_obj )
		{
			const Core::IO::Reader::SerializedObject<T> serializedObject
				= Core::IO::Reader::SerializedObject( *this, p_path, &p_obj );

			serializedObject.read();
		}

		// Directly serialize an object p_obj and write the json file at p_path
		template<typename T>
		void writeObject( const FilePath & p_path, const T & p_obj )
		{
			const Core::IO::Writer::SerializedObject<T> serializedObject
				= Core::IO::Writer::SerializedObject( *this, p_path, &p_obj );

			serializedObject.write();
		}
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// Access the serializer to serialize / deserialize objects, or directly read / write file in json.
	inline Application::System::Serializer & SERIALIZER()
	{
		return Util::Singleton<Application::System::Serializer>::get();
	}
} // namespace VTX::App

#endif
