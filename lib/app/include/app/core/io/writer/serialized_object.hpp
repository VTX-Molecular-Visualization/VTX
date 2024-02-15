#ifndef __VTX_APP_CORE_IO_WRITER_SERIALIZED_OBJECT__
#define __VTX_APP_CORE_IO_WRITER_SERIALIZED_OBJECT__

#include "app/core/serialization/serialization.hpp"
#include "app/core/serialization/version.hpp"
#include <util/filesystem.hpp>
#include <util/json/io.hpp>
#include <util/json/json.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::IO::Writer
{
	template<typename T>
	class SerializedObject
	{
	  public:
		SerializedObject(
			const Serialization::Serialization & p_serializer,
			const FilePath &					 p_path,
			const T *							 p_obj,
			const Serialization::Version &		 p_version = Serialization::Version::CURRENT
		) :
			_serializer( p_serializer ),
			_path( p_path ), _obj( p_obj ), _version( p_version )
		{
		}

		void write() const
		{
			const VTX::Util::JSon::Document jsonDoc
				= { { "VERSION", _serializer.serialize( _version ) }, { "DATA", _serializer.serialize( *_obj ) } };

			Util::JSon::IO::write( _path, jsonDoc );
		}

	  private:
		const Serialization::Serialization & _serializer;
		FilePath							 _path;
		Serialization::Version				 _version;

		const T * _obj;
	};

} // namespace VTX::App::Core::IO::Writer

#endif
