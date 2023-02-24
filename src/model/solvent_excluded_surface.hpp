#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "mesh_triangle.hpp"
#include "model/selection.hpp"
#include "object3d/helper/grid.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Category;
		class SolventExcludedSurface : public MeshTriangle
		{
			VTX_MODEL

		  public:
			struct Range
			{
				uint first;
				uint count;
			};

			inline const static float PROBE_RADIUS = 1.4f;
			inline const static float VOXEL_SIZE   = 0.4f;

			inline const Model::Category * const getCategory() const { return _category; }
			inline const std::vector<Range> &	 getAtomsToTriangles() const { return _atomsToTriangles; }
			const Math::Transform &				 getTransform() const override;

			void refresh();
			void refreshColors();
			void refreshVisibilities();
			void refreshSelections();

		  protected:
			void _init() override;
			void _instantiate3DViews() override;

		  private:
			struct SESGridData
			{
				float sdf;
				int	  nearestAtom;
			};

			const Model::Category * const _category;

			std::vector<Range> _atomsToTriangles;
			// Move to base class?
			uint _indiceCount = 0;

			SolventExcludedSurface( const Category * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
