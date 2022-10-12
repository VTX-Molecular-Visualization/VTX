#ifndef __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_MODEL_SOLVENT_EXCLUDED_SURFACE__

#include "mesh_triangle.hpp"
#include "model/selection.hpp"
#include "object3d/helper/grid.hpp"

namespace VTX
{
	namespace Model
	{
		using namespace VTX::Renderer::GL;

		class Molecule;
		class SolventExcludedSurface : public MeshTriangle
		{
			VTX_MODEL

		  public:
			inline Model::Molecule * const					  getMolecule() { return _molecule; }
			inline const std::vector<std::pair<uint, uint>> & getAtomsToTriangles() const { return _atomsToTriangles; }
			const Math::Transform &							  getTransform() const override;

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

			Mode _mode = Mode::CPU;

			// CPU.
			Model::Molecule * const			   _molecule;
			std::vector<std::pair<uint, uint>> _atomsToTriangles;

			void _initCPU();

			// GPU.
			void _initGPU();

			SolventExcludedSurface( Molecule * const );
			~SolventExcludedSurface() = default;
		};
	} // namespace Model
} // namespace VTX

#endif
