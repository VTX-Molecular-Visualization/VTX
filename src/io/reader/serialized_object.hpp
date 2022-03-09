#ifndef __VTX_READER_SERIALIZED_OBJECT__
#define __VTX_READER_SERIALIZED_OBJECT__

#include "base_reader.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include <QTextStream>
#include <nlohmann/json.hpp>

namespace VTX::IO::Reader
{
	template<typename T>
	class SerializedObject : public BaseReader<T>
	{
	  public:
		void readFile( const IO::FilePath & p_path, T & p_data ) override
		{
			IO::Serializer serializer = IO::Serializer();

			QFile file( p_path.qpath() );
			if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
			{
				throw Exception::IOException( "Can not read file: " + p_path.path() );
			}

			QTextStream			 in( &file );
			const QString		 str  = in.readAll();
			const nlohmann::json json = nlohmann::json::parse( str.toStdString() );
			serializer.deserialize( json.at( "DATA" ),
									std::make_tuple( json.at( "_VERSION" ).at( "MAJOR" ),
													 json.at( "_VERSION" ).at( "MINOR" ),
													 json.at( "_VERSION" ).at( "REVISION" ) ),
									p_data );
			file.close();
		}
	};
} // namespace VTX::IO::Reader
#endif
