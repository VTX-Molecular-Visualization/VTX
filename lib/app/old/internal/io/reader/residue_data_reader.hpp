#ifndef __VTX_APP_OLD_INTERNAL_IO_READER_RESIDUE_DATA_READER__
#define __VTX_APP_OLD_INTERNAL_IO_READER_RESIDUE_DATA_READER__

#include "app/old/core/io/reader/base_reader.hpp"
#include <core/chemdb/bond.hpp>
#include <core/chemdb/category.hpp>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace VTX::App::Old::Internal::IO::Reader
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

		std::string						  fullname;
		VTX::Core::ChemDB::Category::TYPE category;
		std::vector<BondData>			  bondData;
	};

	class ResidueDataReader : public Core::IO::Reader::BaseReader<ResidueData>
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
} // namespace VTX::App::Old::Internal::IO::Reader
#endif