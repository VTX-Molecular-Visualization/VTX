#include "renderer/geometry/ses.hpp"
#include "bcs/sesdf/sesdf.hpp"

namespace VTX::Renderer::Geometry
{
	struct SES::Construction
	{
		bcs::sesdf::SesdfData	  data;
		bcs::sesdf::SesdfGraphics graphics;
	};

	SES::SES()
	{
		convexPatches.vertexLayout	 = "SES.ConvexPatches";
		convexPatches.indirectBuffer = "Indirect.SES.ConvexPatches";

		circlePatches.vertexLayout	 = "SES.CirclePatches";
		circlePatches.indirectBuffer = "Indirect.SES.CirclePatches";

		segmentPatches.vertexLayout	  = "SES.SegmentPatches";
		segmentPatches.indirectBuffer = "Indirect.SES.SegmentPatches";

		concavePatches.vertexLayout	  = "SES.ConcavePatches";
		concavePatches.indirectBuffer = "Indirect.SES.ConcavePatches";
	}

	SES::~SES() = default;

	void SES::construct( const Desc::Handle p_handle, const SystemData & p_data )
	{ _construction.try_emplace( p_handle, std::make_unique<Construction>() ); }
} // namespace VTX::Renderer::Geometry
