#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "buffer/solvent_excluded_surface.hpp"
#include "mesh_triangle.hpp"
#include "object3d/helper/grid.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SolventExcludedSurface : public MeshTriangle
		{
			VTX_MODEL

		  public:
			inline Model::Molecule * const	getMolecule() { return _molecule; }
			inline Object3D::Helper::Grid & getGridAtoms() { return _gridAtoms; }
			inline Object3D::Helper::Grid & getGridSES() { return _gridSES; }

		  protected:
			void _init() override;

		  private:
			Model::Molecule * const _molecule;
			Object3D::Helper::Grid	_gridAtoms;
			Object3D::Helper::Grid	_gridSES;

			SolventExcludedSurface( Molecule * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
