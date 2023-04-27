#ifndef __VTX_APP_COMPONENT_CHEMISTRY_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_APP_COMPONENT_CHEMISTRY_SOLVENT_EXCLUDED_SURFACE__

#include "_fwd.hpp"
#include "app/model/mesh_triangle.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/object3d/helper/grid.hpp"
#include "app/old_app/renderer/gl/buffer.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Chemistry
{
	class SolventExcludedSurface : public Model::MeshTriangle
	{
		VTX_MODEL

	  public:
		struct Range
		{
			uint first;
			uint count;
		};

		inline const static float PROBE_RADIUS = 1.4f;

		inline const Component::Chemistry::Category * const getCategory() const { return _category; }
		inline const std::vector<Range> &					getAtomsToTriangles() const { return _atomsToTriangles; }
		const Math::Transform &								getTransform() const override;

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

		const Chemistry::Category * const _category;

		std::vector<Range> _atomsToTriangles;

		// Move to base class?
		uint _indiceCount = 0;

		SolventExcludedSurface( const Category * const );
		~SolventExcludedSurface() = default;
	};
} // namespace VTX::App::Component::Chemistry

#endif