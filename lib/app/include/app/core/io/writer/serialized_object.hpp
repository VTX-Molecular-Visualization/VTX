#ifndef __VTX_APP_CORE_IO_WRITER_SERIALIZED_OBJECT__
#define __VTX_APP_CORE_IO_WRITER_SERIALIZED_OBJECT__

#include "app/core/serialization/version.hpp"
#include "app/vtx_app.hpp"
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
			const FilePath &			   p_path,
			const T *					   p_obj,
			const Serialization::Version & p_version = Serialization::Version::CURRENT
		) :
			_path( p_path ),
			_obj( p_obj ), _version( p_version )
		{
		}

		void write()
		{
			const VTX::Util::JSon::Document jsonDoc
				= { { "VERSION", SERIALIZER().serialize( _version ) }, { "DATA", SERIALIZER().serialize( *_obj ) } };

			Util::JSon::IO::write( _path, jsonDoc );
		}

	  private:
		FilePath			   _path;
		Serialization::Version _version;

		const T * _obj;
	};

} // namespace VTX::App::Core::IO::Writer

#endif
