#ifndef __VTX_MODEL_CONTOUR_BUILDUP__
#define __VTX_MODEL_CONTOUR_BUILDUP__

#include "base_model_3d.hpp"
#include "buffer/contour_buildup.hpp"
#include "math/grid.hpp"

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

			void render( const Object3D::Camera & p_camera ) const override;

			void print() const;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

		  private:
			// ??
			const uint MaxAtomNeighborNb  = 64;
			const uint MaxProbePerCircle  = 64;
			const uint MaxProbeNeighborNb = 64;

			struct GridCellInfo
			{
				int start;
				int end;
				int pad[ 2 ];
			};

			struct AtomInformation
			{
				Vec3f position;
				float radius;
				uint  startCircleIdx;
				uint  neighborNb;
				uint  gridHash;
				int	  pad;
			};

			struct CircleIntersection
			{
				Vec3f center;
				float radius;
				uint  atomId;
				uint  neighboorAtomId;
				uint  startProbeIdx;
				uint  probeNb;
			};

			Model::Molecule * const _molecule;
			const float				_probeRadius;
			Math::Grid				_accelerationGrid = Math::Grid();

			std::vector<AtomInformation> _atomsInformations = std::vector<AtomInformation>();

			ContourBuildup( Molecule * const );
			~ContourBuildup() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
