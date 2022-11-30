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

			inline Model::Molecule * const	  getMolecule() { return _molecule; }
			inline const std::vector<Range> & getAtomsToTriangles() const { return _atomsToTriangles; }
			const Math::Transform &			  getTransform() const override;

			void refresh();
			void refreshColors();
			void refreshVisibilities();
			void refreshSelection( const Model::Selection::MapChainIds * const p_selection = nullptr );

		  protected:
			void _init() override;
			void _instantiate3DViews() override;

		  private:
			enum class Mode
			{
				CPU,
				GPU
			};

			struct AtomGridData
			{
				int index;
			};

			struct SESGridData
			{
				float sdf;
				int	  nearestAtom;
			};

			Mode _mode = Mode::GPU;

			Model::Molecule * const _molecule;
			std::vector<Range>		_atomsToTriangles;
			// Move to base class?
			uint _indiceCount = 0;

			void _refreshCPU();
			void _refreshGPU();

			SolventExcludedSurface( Molecule * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
