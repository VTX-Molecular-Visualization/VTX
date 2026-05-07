#include "renderer/geometry/ses.hpp"
#include "renderer/representation.hpp"
#include <util/logger.hpp>

#ifdef VTX_CUDA_ENABLED
#include "renderer/geometry/ses_cuda.hpp"
#endif

namespace VTX::Renderer::Geometry
{
	struct SES::Construction
	{
#ifdef VTX_CUDA_ENABLED
		SESDetail::CudaConstructionPtr cudaConstruction;
#endif

		uint32_t convexPatchNb	= 0;
		uint32_t circlePatchNb	= 0;
		uint32_t segmentPatchNb = 0;
		uint32_t concavePatchNb = 0;
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
	{
		auto construction = std::make_unique<Construction>();

		const Index atomCount = p_data.data.getAtomCount();
		if ( atomCount != 0 && p_data.trajectory.size() != atomCount )
		{
			VTX_WARNING(
				"Can not build SES for system {}: atom count ({}) and position count ({}) mismatch.",
				p_data.uid,
				atomCount,
				p_data.trajectory.size()
			);
		}
		else if ( atomCount != 0 )
		{
#ifdef VTX_CUDA_ENABLED
			SESDetail::CudaBuildResult result = SESDetail::buildCudaConstruction(
				p_data.trajectory, p_data.data.atomSymbols, SES_PROBE_RADIUS_DEFAULT
			);

			construction->cudaConstruction = std::move( result.construction );
			construction->convexPatchNb	   = result.convexPatchNb;
			construction->circlePatchNb	   = result.circlePatchNb;
			construction->segmentPatchNb   = result.segmentPatchNb;
			construction->concavePatchNb   = result.concavePatchNb;
#endif
		}

		convexPatches.construct( p_handle, construction->convexPatchNb );
		circlePatches.construct( p_handle, construction->circlePatchNb );
		segmentPatches.construct( p_handle, construction->segmentPatchNb );
		concavePatches.construct( p_handle, construction->concavePatchNb );

		_construction.emplace( p_handle, std::move( construction ) );
	}

	void SES::resize( Context::ContextWrapper & p_context )
	{
		p_context.setBuffer<std::array<uint32_t, 2>>(
			"SES.ConvexPatches.Elements", std::max<uint32_t>( 1u, convexPatches.count )
		);
		p_context.setBuffer<std::array<uint32_t, 2>>(
			"SES.CirclePatches.Atoms", std::max<uint32_t>( 1u, circlePatches.count )
		);
		p_context.setBuffer<std::array<uint32_t, 4>>(
			"SES.SegmentPatches.Ids", std::max<uint32_t>( 1u, segmentPatches.count )
		);
	}

	void SES::clear()
	{
		convexPatches.clear();
		circlePatches.clear();
		segmentPatches.clear();
		concavePatches.clear();

		_construction.clear();
	}

	void SES::uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
	{
		(void)p_context;
		(void)p_handle;
	}
} // namespace VTX::Renderer::Geometry
