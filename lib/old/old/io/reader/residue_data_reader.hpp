#ifndef __VTX_READER_RESIDUE_DATA__
#define __VTX_READER_RESIDUE_DATA__

#include "base_reader.hpp"
#include "model/bond.hpp"
#include "model/category_enum.hpp"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace VTX::IO::Reader
{
	class BondData
	{
	  public:
		std::string		   atom1;
		std::string		   atom2;
		Model::Bond::ORDER bondOrder;
	};
	class ResidueData
	{
	  public:
		static const ResidueData DEFAULT;

		ResidueData() {};
		ResidueData( const std::string & p_fullname, const std::vector<BondData> & p_bondData ) :
			fullname( p_fullname ), bondData( p_bondData ) {};

		std::string			  fullname;
		CATEGORY_ENUM		  category;
		std::vector<BondData> bondData;
	};

	class ResidueDataReader : BaseReader<ResidueData>
	{
	  public:
		bool readResidueData( const std::string & p_residueSymbol, ResidueData & p_residueData );

	  protected:
		virtual void readFile( const IO::FilePath &, ResidueData & ) { throw Exception::NotImplementedException(); }
		virtual void readBuffer( const std::string & p_buffer, const IO::FilePath &, ResidueData & )
		{
			throw Exception::NotImplementedException();
		}

	  private:
		void _goToResidue( std::stringstream & p_stream, const std::string & p_residueSymbol );
	};
} // namespace VTX::IO::Reader
#endif
