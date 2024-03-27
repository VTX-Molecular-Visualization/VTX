#ifndef __VTX_APP_APPLICATION_SYSTEM_SERIALIZER__
#define __VTX_APP_APPLICATION_SYSTEM_SERIALIZER__

#include "app/application/system/system_registration.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/core/io/writer/serialized_object.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/core/system/base_system.hpp"

namespace VTX::App::Application::System
{
	class Serializer final : public Core::System::BaseSystem, public Core::Serialization::Serialization
	{
	  public:
		inline static const SystemRegistration<Serializer> SYSTEM = SystemRegistration<Serializer>();

	  public:
		template<typename T>
		void readObject( const FilePath & p_path, T & p_obj )
		{
			const Core::IO::Reader::SerializedObject<T> serializedObject
				= Core::IO::Reader::SerializedObject( *this, p_path, &p_obj );

			serializedObject.read();
		}

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
	Application::System::Serializer & SERIALIZER();
} // namespace VTX::App

#endif
