#ifndef __VTX_READER_SERIALIZED_OBJECT__
#define __VTX_READER_SERIALIZED_OBJECT__

#include "base_reader.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include <nlohmann/json.hpp>
#include <QTextStream>

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
			serializer.deserialize( json.at( "DATA" ), p_data );
			file.close();
		}
	};
} // namespace VTX::IO::Reader
#endif
