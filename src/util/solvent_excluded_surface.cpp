#include "util/solvent_excluded_surface.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"
#include "model/solvent_excluded_surface.hpp"
#include "tool/logger.hpp"

namespace VTX::Util::SolventExcludedSurface
{
	std::pair<bool, size_t> checkSESMemory( const Model::Category & p_category )
	{
		memory_size_type memory = 0;

		// Estimate memory needed from grid size.
		const std::vector<uint>		 atomsIdx = p_category.generateAtomIndexList();
		const Object3D::Helper::AABB molAABB  = p_category.getAABB();
		const float					 maxVdWRadius
			= *std::max_element( Model::Atom::SYMBOL_VDW_RADIUS,
								 Model::Atom::SYMBOL_VDW_RADIUS + std::size( Model::Atom::SYMBOL_VDW_RADIUS ) );

		const float			   atomGridCellSize = Model::SolventExcludedSurface::PROBE_RADIUS + maxVdWRadius;
		const Vec3f			   gridMin			= molAABB.getMin() - atomGridCellSize;
		const Vec3f			   gridMax			= molAABB.getMax() + atomGridCellSize;
		const Vec3f			   gridSize			= gridMax - gridMin;
		Vec3i				   atomGridSize		= Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );
		Object3D::Helper::Grid gridAtoms = Object3D::Helper::Grid( gridMin, Vec3f( atomGridCellSize ), atomGridSize );
		Vec3i sesGridSize = Vec3i( Util::Math::ceil( gridSize / Model::SolventExcludedSurface::VOXEL_SIZE ) );
		Object3D::Helper::Grid gridSES
			= Object3D::Helper::Grid( gridMin, Vec3f( Model::SolventExcludedSurface::VOXEL_SIZE ), sesGridSize );

		// bufferSesGridData
		memory_size_type sizeGridData = sizeof( float ) + sizeof( int );
		memory						  = gridSES.getCellCount() * sizeGridData;
		// bufferAtomGridDataSorted
		memory += gridAtoms.getCellCount() * sizeof( Model::SolventExcludedSurface::Range );
		// bufferAtomIndexSorted
		memory += p_category.getMolecule()->getAtomCount() * sizeof( uint );
		// bufferAtomPosition
		memory += p_category.getMolecule()->getAtomCount() * sizeof( Vec4f );
		// From tests, vram is the most used in marching cube or grid reduction (2 - 2.5x)
		memory *= 2;

		VTX_DEBUG( "Estimated needed vram: {}mo", memory / 1000000.f );

		return { memory > SMALL_SES_MEMORY_THRESHOLD, memory };
	}

} // namespace VTX::Util::SolventExcludedSurface
