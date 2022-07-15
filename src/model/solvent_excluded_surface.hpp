#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "base_model_3d.hpp"
#include "buffer/solvent_excluded_surface.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SolventExcludedSurface : public BaseModel3D<Buffer::SolventExcludedSurface>
		{
			VTX_MODEL

		  public:
			inline Model::Molecule * const getMolecule() { return _molecule; }

			void print() const;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

		  private:
			Model::Molecule * const _molecule;

			SolventExcludedSurface( Molecule * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
