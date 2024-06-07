#include <string>
#include <vector>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
//
#include "tools/mdprep/gromacs/editconf.hpp"
#include "tools/mdprep/gromacs/pdb2gmx.hpp"

namespace VTX::Tool::Mdprep::Gateway
{
	void boxShapeCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_EDITCONF_BOX_SHAPE::COUNT ); it_idx++ )
			p_out.push_back( string( static_cast<E_EDITCONF_BOX_SHAPE>( it_idx ) ) );
	}
	void waterModelCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_WATER_MODEL::COUNT ); it_idx++ )
			p_out.push_back( string( static_cast<E_WATER_MODEL>( it_idx ) ) );
	}
} // namespace VTX::Tool::Mdprep::Gateway
