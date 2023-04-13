#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "model/math/grid.hpp"
#include "model/object3d/mesh_triangle.hpp"
#include "model/selection/selection.hpp"

namespace VTX
{
	namespace Model
	{
		// TODO_VTX_LOGIC : What is it ?
		// using namespace VTX::Renderer::GL;

		class Molecule;
		class SolventExcludedSurface : public MeshTriangle
		{
			VTX_MODEL

		  public:
			inline const static float PROBE_RADIUS = 1.4f;
			inline const static float VOXEL_SIZE   = 0.4f;

			inline Model::Molecule * const					  getMolecule() { return _molecule; }
			inline const std::vector<std::pair<uint, uint>> & getAtomsToTriangles() const { return _atomsToTriangles; }
			const Math::Transform &							  getTransform() const override;

			void refresh();
			void refreshColors();
			void refreshVisibilities();
			void refreshSelection( const Model::Selection::MapChainIds * const p_selection = nullptr );

		  protected:
			void _init() override;

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

			Mode _mode = Mode::CPU;

			// CPU.
			Model::Molecule * const			   _molecule;
			std::vector<std::pair<uint, uint>> _atomsToTriangles;

			void _refreshCPU();

			// GPU.
			void _refreshGPU();

			SolventExcludedSurface( Molecule * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
