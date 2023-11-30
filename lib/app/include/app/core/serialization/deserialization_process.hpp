#ifndef __VTX_APP_CORE_SERIALIZATION_DESERIALIZATION_PROCESS__
#define __VTX_APP_CORE_SERIALIZATION_DESERIALIZATION_PROCESS__

#include "app/core/serialization/serialization.hpp"
#include "app/core/serialization/version.hpp"
#include "app/vtx_app.hpp"
#include <util/exceptions.hpp>
#include <util/json/io.hpp>
#include <util/json/json.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Serialization
{
	template<typename T>
	class DeserializationProcess
	{
	  public:
		DeserializationProcess( const FilePath & p_path, T * const p_target ) : _path( p_path ), _target( p_target ) {}

		void run()
		{
			Util::JSon::Document fullDoc = Util::JSon::IO::open( _path );

			if ( !fullDoc.json().contains( "VERSION" ) || !fullDoc.json().contains( "DATA" ) )
				throw IOException( "Ill-formed save file" );

			Version fileVersion;
			SERIALIZER().deserialize( fullDoc.json()[ "VERSION" ], fileVersion );

			if ( fileVersion > Version::CURRENT )
				throw IOException( "Can not read file, version is newer than VTX" );

			VTX::Util::JSon::BasicJSon & dataJSon = fullDoc.json()[ "DATA" ];

			if ( fileVersion < Version::CURRENT )
				SERIALIZER().migrate( dataJSon, *_target, fileVersion );

			SERIALIZER().deserialize( dataJSon, *_target );
		}

	  private:
		FilePath _path;
		T *		 _target;
	};
} // namespace VTX::App::Core::Serialization

#endif
