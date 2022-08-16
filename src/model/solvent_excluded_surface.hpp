#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "base_model_3d.hpp"
#include "buffer/solvent_excluded_surface.hpp"
#include "object3d/helper/grid.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SolventExcludedSurface : public BaseModel3D<Buffer::SolventExcludedSurface>
		{
			VTX_MODEL

		  public:
			inline Model::Molecule * const	getMolecule() { return _molecule; }
			inline Object3D::Helper::Grid & getGridAtoms() { return _gridAtoms; }
			inline Object3D::Helper::Grid & getGridSES() { return _gridSES; }

			void print() const;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

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
