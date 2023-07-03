#ifndef __VTX_IO_READER_RESIDUE_DATA_READER__
#define __VTX_IO_READER_RESIDUE_DATA_READER__

#include "io/core/reader/base_reader.hpp"
#include <core/chemdb/bond.hpp>
#include <core/chemdb/category.hpp>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace VTX::IO::Reader
{
	class BondData
	{
	  public:
		std::string					   atom1;
		std::string					   atom2;
		VTX::Core::ChemDB::Bond::ORDER bondOrder;
	};
	class ResidueData
	{
	  public:
		static const ResidueData DEFAULT;

		ResidueData() {};
		ResidueData( const std::string & p_fullname, const std::vector<BondData> & p_bondData ) :
			fullname( p_fullname ), bondData( p_bondData ) {};

		std::string						  fullname = "";
		VTX::Core::ChemDB::Category::TYPE category = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;
		std::vector<BondData>			  bondData = std::vector<BondData>();
	};

	class ResidueDataReader : public IO::Core::Reader::BaseReader<ResidueData>
	{
	  public:
		bool readResidueData( const std::string & p_residueSymbol, ResidueData & p_residueData );

	  protected:
		virtual void readFile( const FilePath &, ResidueData & ) { throw NotImplementedException(); }
		virtual void readBuffer( const std::string & p_buffer, const FilePath &, ResidueData & )
		{
			throw NotImplementedException();
		}

	  private:
		void _goToResidue( std::stringstream & p_stream, const std::string & p_residueSymbol );
	};
} // namespace VTX::IO::Reader
#endif
