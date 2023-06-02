#include "core/struct/solvent_excluded_surface.hpp"

namespace VTX::Core::Struct
{
	SolventExcludedSurface::SolventExcludedSurface() {}

	void SolventExcludedSurface::appendRange( const Range & p_range )
	{
		_atomsToTriangles.emplace_back( p_range );
		_indiceCount += p_range.getCount();
	}
} // namespace VTX::Core::Struct
