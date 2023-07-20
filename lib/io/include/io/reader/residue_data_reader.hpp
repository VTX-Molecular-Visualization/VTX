#ifndef __VTX_IO_READER_RESIDUE_DATA_READER__
#define __VTX_IO_READER_RESIDUE_DATA_READER__

#include "io/core/reader/base_reader.hpp"
#include "io/struct/residue_data.hpp"
#include <core/chemdb/bond.hpp>
#include <core/chemdb/category.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace VTX::IO::Reader
{
	class ResidueDataReader : public IO::Core::Reader::BaseReader<Struct::ResidueData>
	{
	  public:
		bool readResidueData( const std::string & p_residueSymbol, Struct::ResidueData & p_residueData );

	  protected:
		virtual void readFile( const FilePath &, Struct::ResidueData & ) { throw NotImplementedException(); }
		virtual void readBuffer( const std::string & p_buffer, const FilePath &, Struct::ResidueData & )
		{
			throw NotImplementedException();
		}

	  private:
		void _goToResidue( std::stringstream & p_stream, const std::string & p_residueSymbol );
	};
} // namespace VTX::IO::Reader
#endif
