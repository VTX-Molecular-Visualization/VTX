#ifndef __VTX_APP_CORE_SERIALIZATION_SERIALIZATION_PROCESS__
#define __VTX_APP_CORE_SERIALIZATION_SERIALIZATION_PROCESS__

#include "app/core/serialization/version.hpp"
#include "app/vtx_app.hpp"
#include <util/json/io.hpp>
#include <util/json/json.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Serialization
{
	template<typename T>
	class SerializationProcess
	{
	  public:
		SerializationProcess( const FilePath & p_path, const T * p_obj, const Version & p_version = Version::CURRENT ) :
			_path( p_path ), _obj( p_obj ), _version( p_version )
		{
		}

		void run()
		{
			const VTX::Util::JSon::Document jsonDoc
				= { { "VERSION", SERIALIZER().serialize( _version ) }, { "DATA", SERIALIZER().serialize( *_obj ) } };

			Util::JSon::IO::write( _path, jsonDoc );
		}

	  private:
		FilePath _path;
		Version	 _version;

		const T * _obj;
	};

} // namespace VTX::App::Core::Serialization

#endif
