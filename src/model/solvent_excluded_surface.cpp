#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"

namespace VTX
{
	namespace Model
	{
		SolventExcludedSurface::SolventExcludedSurface( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_SOLVENT_EXCLUDED_SURFACE ), _molecule( p_molecule )
		{
		}

		void SolventExcludedSurface::print() const { VTX_DEBUG( "ICI" ); }

		void SolventExcludedSurface::_init()
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating SES..." );

			//// Step 1: sort atoms in grid.
			const float probeRadius	 = 1.4f;
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );

			_grid								   = Object3D::Helper::Grid();
			const Object3D::Helper::AABB & molAABB = _molecule->getAABB();

			const float cellSize = probeRadius + maxVdWRadius;
			_grid.worldOrigin	 = molAABB.getMin() - cellSize;

			const Vec3f		  worldSize = Util::Math::abs( molAABB.getMax() + cellSize - _grid.worldOrigin );
			const std::size_t gridSize
				= Util::Math::nextPowerOfTwoValue( Util::Math::nextPowerOfTwoExponent( _molecule->getAtomCount() ) );

			_grid.size	   = Vec3i( static_cast<int>( gridSize ) );
			_grid.cellSize = Vec3f( worldSize / Vec3f( _grid.size ) );

			const auto &			   atoms		  = _molecule->getAtoms();
			const std::vector<Vec3f> & atomsPositions = _molecule->getAtomPositionFrame( 0 );

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::_fillBuffer() {}

		void SolventExcludedSurface::_computeAABB() const {}

		void SolventExcludedSurface::_instantiate3DViews() {}

	} // namespace Model
} // namespace VTX
