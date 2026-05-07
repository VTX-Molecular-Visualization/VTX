#include "bcs/sesdf/sesdf.hpp"
#include "renderer/geometry/ses_cuda.hpp"
#include <glm/common.hpp>

namespace VTX::Renderer::Geometry::SESDetail
{
	struct CudaConstruction
	{
		std::vector<bcs::Vec4f>		molecule;
		std::unique_ptr<bcs::Sesdf> ses;
	};

	void CudaConstructionDeleter::operator()( CudaConstruction * p_construction ) const { delete p_construction; }

	namespace
	{
		bcs::Aabb _computeAabb( const std::span<const Vec3f> p_positions )
		{
			bcs::Aabb aabb { p_positions.front(), p_positions.front() };

			for ( const Vec3f & position : p_positions )
			{
				aabb.min = glm::min( aabb.min, position );
				aabb.max = glm::max( aabb.max, position );
			}

			return aabb;
		}
	} // namespace

	CudaBuildResult buildCudaConstruction(
		const std::span<const Vec3f>					  p_positions,
		const std::span<const Core::ChemDB::Atom::SYMBOL> p_symbols,
		const float										  p_probeRadius
	)
	{
		CudaBuildResult result;

		const size_t atomCount = p_positions.size();
		if ( atomCount == 0 )
		{
			return result;
		}

		auto construction = std::make_unique<CudaConstruction>();
		construction->molecule.reserve( atomCount );

		for ( size_t atomIndex = 0; atomIndex < atomCount; ++atomIndex )
		{
			const Vec3f & position = p_positions[ atomIndex ];
			const auto	  symbol   = p_symbols[ atomIndex ];
			const float	  radius   = Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ size_t( symbol ) ];

			construction->molecule.emplace_back( position, radius );
		}

		const bcs::Aabb aabb = _computeAabb( p_positions );
		construction->ses	 = std::make_unique<bcs::Sesdf>(
			bcs::ConstSpan<bcs::Vec4f>( construction->molecule ), aabb, p_probeRadius, true, true
		);

		const bcs::sesdf::SesdfGraphics graphics = construction->ses->getGraphics();

		result.convexPatchNb  = graphics.convexPatchNb;
		result.circlePatchNb  = graphics.circlePatchNb;
		result.segmentPatchNb = graphics.segmentPatchNb;
		result.concavePatchNb = graphics.concavePatchNb;

		result.construction = CudaConstructionPtr( construction.release() );

		return result;
	}
} // namespace VTX::Renderer::Geometry::SESDetail
