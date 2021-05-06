#ifndef __VTX_READER_SERIALIZED_OBJECT__
#define __VTX_READER_SERIALIZED_OBJECT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace VTX::IO::Reader
{
	template<typename T>
	class SerializedObject : public BaseReader<T>
	{
	  public:
		void readFile( const FilePath & p_path, T & p_data ) override
		{
			IO::Serializer serializer = IO::Serializer();
			std::ifstream  is( p_path );
			nlohmann::json json;
			is >> json;
			serializer.deserialize( json, p_data );
		}
	};
} // namespace VTX::IO::Reader
#endif
