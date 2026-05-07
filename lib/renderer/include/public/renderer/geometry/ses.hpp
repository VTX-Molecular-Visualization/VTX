#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"
#include <array>
#include <memory>

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		class PatchGeometry : public BaseGeometry
		{
		  public:
			void construct( const Desc::Handle p_handle, const Index p_count )
			{ _addRange( p_handle, p_count, p_count ); }
		};

		SES();
		~SES();

		PatchGeometry convexPatches;
		PatchGeometry circlePatches;
		PatchGeometry segmentPatches;
		PatchGeometry concavePatches;

		struct Construction;

		void construct( const Desc::Handle p_handle, const SystemData & p_data );

		void resize( Context::ContextWrapper & p_context );

		void clear();

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle );

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
