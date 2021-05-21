#ifndef __VTX_WRITER_SERIALIZED_OBJECT__
#define __VTX_WRITER_VTX__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_writer.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace VTX::IO::Writer
{
	template<typename T>
	class SerializedObject : public BaseWriter<T>
	{
	  public:
		void writeFile( const FilePath & p_path, const T & p_data ) override
		{
			IO::Serializer serializer = IO::Serializer();

			nlohmann::json json = { { "_VERSION",
									  { { "MAJOR", VTX_VERSION_MAJOR },
										{ "MINOR", VTX_VERSION_MINOR },
										{ "REVISION", VTX_VERSION_REVISION } } },
									{ "DATA", serializer.serialize( p_data ) } };

			std::ofstream os( p_path );
			os << std::setw( 4 ) << json << std::endl;
			os.close();
		}
	};
} // namespace VTX::IO::Writer
#endif
