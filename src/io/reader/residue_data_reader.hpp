#ifndef __VTX_READER_RESIDUE_DATA__
#define __VTX_READER_RESIDUE_DATA__

#include "base_reader.hpp"
#include "model/bond.hpp"
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
		std::string			  fullname;
		std::vector<BondData> bondData;
	};

	class ResidueDataReader : BaseReader<ResidueData>
	{
	  protected:
		inline static std::map<std::string, ResidueData> mapLoadedResidueData = std::map<std::string, ResidueData>();

	  public:
		void readResidueData( const std::string & p_residueSymbol, ResidueData & p_residueData );

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
