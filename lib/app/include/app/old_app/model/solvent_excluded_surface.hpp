#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "mesh_triangle.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/object3d/helper/grid.hpp"
#include "app/old_app/renderer/gl/buffer.hpp"

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

			inline const Model::Category * const getCategory() const { return _category; }
			inline const std::vector<Range> &	 getAtomsToTriangles() const { return _atomsToTriangles; }
			const Math::Transform &				 getTransform() const override;

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
