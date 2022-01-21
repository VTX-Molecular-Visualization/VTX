#ifndef __VTX_MODEL_CONTOUR_BUILDUP__
#define __VTX_MODEL_CONTOUR_BUILDUP__

#include "base_model_3d.hpp"
#include "buffer/contour_buildup.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class ContourBuildup : public BaseModel3D<Buffer::ContourBuildup>
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

			ContourBuildup( Molecule * const );
			~ContourBuildup() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
