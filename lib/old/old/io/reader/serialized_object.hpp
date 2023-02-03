#ifndef __VTX_READER_SERIALIZED_OBJECT__
#define __VTX_READER_SERIALIZED_OBJECT__

#include "base_reader.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include <QFile>
#include <QTextStream>
#include <nlohmann/json.hpp>

namespace VTX::IO::Reader
{
	template<typename T>
	class SerializedObject : public BaseReader<T>
	{
	  public:
		void readFile( const Util::FilePath & p_path, T & p_data ) override
		{
			IO::Serializer serializer = IO::Serializer();

			QFile file( QString::fromStdString( p_path.path() ) );
			if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
			{
				throw Exception::IOException( "Can not read file: " + p_path.path() );
			}

			QTextStream			 in( &file );
			const QString		 str  = in.readAll();
			const nlohmann::json json = nlohmann::json::parse( str.toStdString() );

			const uint major	  = json.at( "_VERSION" ).at( "MAJOR" );
			const uint minor	  = json.at( "_VERSION" ).at( "MINOR" );
			const uint revision	  = json.at( "_VERSION" ).at( "REVISION" );
			bool	   needUpdate = false;
			if ( major > VTX_VERSION_MAJOR )
			{
				needUpdate = true;
			}
			else if ( major == VTX_VERSION_MAJOR && minor > VTX_VERSION_MINOR )
			{
				needUpdate = true;
			}
			else if ( major == VTX_VERSION_MAJOR && minor == VTX_VERSION_MINOR && revision > VTX_VERSION_REVISION )
			{
				needUpdate = true;
			}

			if ( needUpdate )
			{
				throw Exception::IOException( "Can not read file, version is newer than VTX" );
			}

			serializer.deserialize( json.at( "DATA" ), std::make_tuple( major, minor, revision ), p_data );
			file.close();
		}
	};
} // namespace VTX::IO::Reader
#endif
