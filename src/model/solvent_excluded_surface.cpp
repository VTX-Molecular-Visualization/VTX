#include "solvent_excluded_surface.hpp"

namespace VTX
{
	namespace Model
	{
		SolventExcludedSurface::SolventExcludedSurface( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_SOLVENT_EXCLUDED_SURFACE ), _molecule( p_molecule )
		{
		}

		void SolventExcludedSurface::print() const {}

		void SolventExcludedSurface::_init() {}

		void SolventExcludedSurface::_fillBuffer() {}

		void SolventExcludedSurface::_computeAABB() const {}

		void SolventExcludedSurface::_instantiate3DViews() {}

	} // namespace Model
} // namespace VTX
