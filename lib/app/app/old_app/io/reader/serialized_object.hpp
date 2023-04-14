#ifndef __VTX_READER_SERIALIZED_OBJECT__
#define __VTX_READER_SERIALIZED_OBJECT__

#include "base_reader.hpp"
#include "old_app/define.hpp"
#include "old_app/io/serializer.hpp"
// #include <QFile>
// #include <QTextStream>
#include <nlohmann/json.hpp>
#include <util/types.hpp>

namespace VTX::IO::Reader
{
	template<typename T>
	class SerializedObject : public BaseReader<T>
	{
	  public:
		void readFile( const FilePath & p_path, T & p_data ) override
		{
			const nlohmann::json json = _openJson( p_path );

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
				throw IOException( "Can not read file, version is newer than VTX" );
			}

			IO::Serializer serializer = IO::Serializer();
			serializer.deserialize( json.at( "DATA" ), std::make_tuple( major, minor, revision ), p_data );
		}

	  private:
		// TODO reimplement it without Qt
		nlohmann::json _openJson( const FilePath & p_path )
		{
			return {};

			// QFile file( QString::fromStdString( p_path.string() ) );
			// if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
			//{
			//	throw IOException( "Can not read file: " + p_path.string() );
			// }

			// QTextStream		  in( &file );
			// const std::string str = in.readAll().toStdString();
			// file.close();

			// return nlohmann::json::parse( str );
		}
	};
} // namespace VTX::IO::Reader
#endif
