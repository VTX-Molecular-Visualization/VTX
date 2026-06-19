#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_CHROMATIC_ABERRATION__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_CHROMATIC_ABERRATION__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <utility>

namespace VTX::Renderer
{
	constexpr bool	ACTIVE_CHROMAB_DEFAULT = false;
	constexpr float CHROMAB_RED_DEFAULT	   = 0.f;
	constexpr float CHROMAB_GREEN_DEFAULT  = -0.01f;
	constexpr float CHROMAB_BLUE_DEFAULT   = -0.02f;
	constexpr float CHROMAB_RGB_MIN		   = -0.05f;
	constexpr float CHROMAB_RGB_MAX		   = 0.f;

	struct ChromaticAberrationConfig
	{
		float red;
		float green;
		float blue;
	};

	namespace GraphicsConfigs
	{
		inline const ChromaticAberrationConfig CHROMATIC_ABERRATION_DEFAULT { CHROMAB_RED_DEFAULT,
																			  CHROMAB_GREEN_DEFAULT,
																			  CHROMAB_BLUE_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct ChromaticAberration
	{
		inline static const Desc::Key PASS = "ChromaticAberration";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "chromatic_aberration.frag" } )
				.uniform( "Red", CHROMAB_RED_DEFAULT, std::pair { CHROMAB_RGB_MIN, CHROMAB_RGB_MAX } )
				.uniform( "Green", CHROMAB_GREEN_DEFAULT, std::pair { CHROMAB_RGB_MIN, CHROMAB_RGB_MAX } )
				.uniform( "Blue", CHROMAB_BLUE_DEFAULT, std::pair { CHROMAB_RGB_MIN, CHROMAB_RGB_MAX } )
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const ChromaticAberrationConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.red );
			buffer.write( p_config.green );
			buffer.write( p_config.blue );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
