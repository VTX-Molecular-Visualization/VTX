#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <numeric>

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		class PatchGeometry : public BaseGeometry
		{
		  public:
			void construct( const Desc::Handle p_handle, const Index p_count )
			{
				_addRange( p_handle, p_count, p_count );
				totalSize += p_count;

				auto & indices = _indices( p_handle );
				indices.resize( p_count );
				std::iota( indices.begin(), indices.end(), 0 );
			}

			void clear()
			{
				BaseGeometry::clear();
				totalSize = 0;
			}

			Index totalSize = 0;
		};

		SES();
		~SES();

		static constexpr uint32_t MAX_PROBE_NEIGHBOR_NB = 32u;

		inline static const Desc::Key BUFFER_ATOMS				   = "SES.Atoms";
		inline static const Desc::Key BUFFER_SECTORS			   = "SES.Sectors";
		inline static const Desc::Key BUFFER_PROBES				   = "SES.Probes";
		inline static const Desc::Key BUFFER_PROBE_ATOM_INDICES	   = "SES.ProbesAtomIndices";
		inline static const Desc::Key BUFFER_PROBE_NEIGHBORS	   = "SES.ProbeNeighbors";
		inline static const Desc::Key BUFFER_CONVEX_PATCH_ELEMENTS = "SES.ConvexPatches.Elements";
		inline static const Desc::Key BUFFER_CIRCLE_PATCH_ATOMS	   = "SES.CirclePatches.Atoms";
		inline static const Desc::Key BUFFER_SEGMENT_PATCH_IDS	   = "SES.SegmentPatches.Ids";

		inline static const Desc::Key GEOMETRY_CONVEX_PATCHES  = "SES.ConvexPatches";
		inline static const Desc::Key GEOMETRY_CIRCLE_PATCHES  = "SES.CirclePatches";
		inline static const Desc::Key GEOMETRY_SEGMENT_PATCHES = "SES.SegmentPatches";
		inline static const Desc::Key GEOMETRY_CONCAVE_PATCHES = "SES.ConcavePatches";
		inline static const Desc::Key INDIRECT_CONVEX_PATCHES  = "Indirect.SES.ConvexPatches";
		inline static const Desc::Key INDIRECT_CIRCLE_PATCHES  = "Indirect.SES.CirclePatches";
		inline static const Desc::Key INDIRECT_SEGMENT_PATCHES = "Indirect.SES.SegmentPatches";
		inline static const Desc::Key INDIRECT_CONCAVE_PATCHES = "Indirect.SES.ConcavePatches";
		inline static const Desc::Key INDEX_CONVEX_PATCHES	   = "Index.SES.ConvexPatches";
		inline static const Desc::Key INDEX_CIRCLE_PATCHES	   = "Index.SES.CirclePatches";
		inline static const Desc::Key INDEX_SEGMENT_PATCHES	   = "Index.SES.SegmentPatches";
		inline static const Desc::Key INDEX_CONCAVE_PATCHES	   = "Index.SES.ConcavePatches";
		inline static const Desc::Key PASS_COMPUTE			   = "SES.Compute";

		PatchGeometry convexPatches;
		PatchGeometry circlePatches;
		PatchGeometry segmentPatches;
		PatchGeometry concavePatches;

		struct Construction;

		void construct(
			Context::ContextWrapper & p_context,
			Desc::Handle			  p_handle,
			const SystemData &		  p_data,
			uint32_t				  p_inputAtomOffset
		);

		void resize( Context::ContextWrapper & p_context );

		void clear();

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle );

		void compute( Context::ContextWrapper & p_context );

		[[nodiscard]] bool hasPendingCompute() const;

	  protected:
		std::map<Desc::Handle, std::unique_ptr<Construction>> _construction;

	  private:
		void _constructEmptyRanges( Desc::Handle );
		void _clearPatchGeometries();
		void _releaseBuffers( Context::ContextWrapper & );
		void _disableDraws( Context::ContextWrapper & );
		void _markPendingConstructionsAsIncalculable();
		void _discardPendingCompute( Context::ContextWrapper & );
	};

} // namespace VTX::Renderer::Geometry

#endif
