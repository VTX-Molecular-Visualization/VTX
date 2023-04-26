#ifndef __VTX_READER_RESIDUE_DATA__
#define __VTX_READER_RESIDUE_DATA__

#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/enum_category.hpp"
#include "base_reader.hpp"
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
		App::Component::Chemistry::Bond::ORDER bondOrder;
	};
	class ResidueData
	{
	  public:
		static const ResidueData DEFAULT;

		ResidueData() {};
		ResidueData( const std::string & p_fullname, const std::vector<BondData> & p_bondData ) :
			fullname( p_fullname ), bondData( p_bondData ) {};

		std::string			  fullname;
		App::Component::Chemistry::CATEGORY_ENUM		  category;
		std::vector<BondData> bondData;
	};

	class ResidueDataReader : BaseReader<ResidueData>
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
