#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		SES()
		{
			convexPatches.vertexLayout   = "SES.ConvexPatches";
			convexPatches.indirectBuffer = "Indirect.SES.ConvexPatches";

			circlePatches.vertexLayout   = "SES.CirclePatches";
			circlePatches.indirectBuffer = "Indirect.SES.CirclePatches";

			segmentPatches.vertexLayout   = "SES.SegmentPatches";
			segmentPatches.indirectBuffer = "Indirect.SES.SegmentPatches";

			concavePatches.vertexLayout   = "SES.ConcavePatches";
			concavePatches.indirectBuffer = "Indirect.SES.ConcavePatches";
		}

		Desc::Geometry convexPatches;
		Desc::Geometry circlePatches;
		Desc::Geometry segmentPatches;
		Desc::Geometry concavePatches;

		uint32_t countConvexPatches	= 0;
		uint32_t countCirclePatches	= 0;
		uint32_t countSegmentPatches = 0;
		uint32_t countConcavePatches = 0;

		void clear()
		{
			countConvexPatches	= 0;
			countCirclePatches	= 0;
			countSegmentPatches = 0;
			countConcavePatches = 0;
		}

		[[nodiscard]] std::vector<Desc::DrawIndirectCommand> toDrawIndirectCommands( const uint32_t p_count ) const
		{
			if ( p_count == 0 )
			{
				return {};
			}

			return { Desc::DrawIndirectCommand { p_count, 1, 0, 0 } };
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
