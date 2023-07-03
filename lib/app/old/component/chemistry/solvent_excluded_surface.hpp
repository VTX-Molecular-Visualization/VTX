#ifndef __VTX_APP_OLD_COMPONENT_CHEMISTRY_SOLVENT_EXCLUDED_SURFACE__
#define __VTX_APP_OLD_COMPONENT_CHEMISTRY_SOLVENT_EXCLUDED_SURFACE__

#include "_fwd.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/component/object3d/helper/grid.hpp"
#include "app/old/component/object3d/mesh_triangle.hpp"
#include "app/old/render/renderer/gl/buffer.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Component::Chemistry
{
	class SolventExcludedSurface : public App::Old::Component::Object3D::MeshTriangle
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
		const App::Old::Internal::Math::Transform &				getTransform() const override;

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
} // namespace VTX::App::Old::Component::Chemistry

#endif
