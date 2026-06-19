#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_PIXELIZE__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_PIXELIZE__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <util/types.hpp>
#include <utility>

namespace VTX::Renderer
{
	constexpr bool ACTIVE_PIXELIZE_DEFAULT	   = false;
	constexpr uint PIXELIZE_SIZE_DEFAULT	   = 5;
	constexpr uint PIXELIZE_SIZE_MIN		   = 1;
	constexpr uint PIXELIZE_SIZE_MAX		   = 15;
	constexpr bool PIXELIZE_BACKGROUND_DEFAULT = true;

	struct PixelizeConfig
	{
		uint size;
		bool background;
	};

	namespace GraphicsConfigs
	{
		inline const PixelizeConfig PIXELIZE_DEFAULT { PIXELIZE_SIZE_DEFAULT, PIXELIZE_BACKGROUND_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct Pixelize
	{
		inline static const Desc::Key PASS = "Pixelize";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( "Geometry" )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "pixelize.frag" } )
				.uniform(
					"Size",
					PIXELIZE_SIZE_DEFAULT,
					std::pair { static_cast<double>( PIXELIZE_SIZE_MIN ), static_cast<double>( PIXELIZE_SIZE_MAX ) }
				)
				.uniform( "Background", static_cast<unsigned int>( PIXELIZE_BACKGROUND_DEFAULT ) )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const PixelizeConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.size );
			buffer.write( uint32_t( p_config.background ) );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
