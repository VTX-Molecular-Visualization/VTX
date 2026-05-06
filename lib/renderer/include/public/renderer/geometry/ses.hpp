#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"
#include <memory>

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		SES();
		~SES();


		BaseGeometry convexPatches;
		BaseGeometry circlePatches;
		BaseGeometry segmentPatches;
		BaseGeometry concavePatches;

		struct Construction;

		void construct( const Desc::Handle p_handle, const SystemData & p_data );

		void resize( Context::ContextWrapper & p_context )
		{
			convexPatches.resize( p_context );
			circlePatches.resize( p_context );
			segmentPatches.resize( p_context );
			concavePatches.resize( p_context );
		}

		void clear()
		{
			convexPatches.clear();
			circlePatches.clear();
			segmentPatches.clear();
			concavePatches.clear();
		}

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
		{
			convexPatches.uploadIndexes( p_context, p_handle );
			circlePatches.uploadIndexes( p_context, p_handle );
			segmentPatches.uploadIndexes( p_context, p_handle );
			concavePatches.uploadIndexes( p_context, p_handle );
		}

		[[nodiscard]] std::vector<Desc::DrawIndirectCommand> toDrawIndirectCommands( const uint32_t p_count ) const
		{
			if ( p_count == 0 )
			{
				return {};
			}

			return { Desc::DrawIndirectCommand { p_count, 1, 0, 0 } };
		}

	  protected:
		std::map<Desc::Handle, std::unique_ptr<Construction>> _construction;
	};

} // namespace VTX::Renderer::Geometry

#endif
