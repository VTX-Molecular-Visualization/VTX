#ifndef __VTX_RENDERER_BUILDER_POST_PROCESS_OUTLINE__
#define __VTX_RENDERER_BUILDER_POST_PROCESS_OUTLINE__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/context_wrapper.hpp"
#include "renderer/graph_builder.hpp"
#include <util/constants.hpp>
#include <utility>

namespace VTX::Renderer
{
	constexpr bool			ACTIVE_OUTLINE_DEFAULT		= false;
	const Util::Color::Rgba COLOR_OUTLINE_DEFAULT		= COLOR_WHITE;
	constexpr float			OUTLINE_SENSITIVITY_DEFAULT = 0.5f;
	constexpr float			OUTLINE_SENSITIVITY_MIN		= 0.f;
	constexpr float			OUTLINE_SENSITIVITY_MAX		= 1.f;
	constexpr uint			OUTLINE_THICKNESS_DEFAULT	= 1;
	constexpr uint			OUTLINE_THICKNESS_MIN		= 1;
	constexpr uint			OUTLINE_THICKNESS_MAX		= 5;

	struct OutlineConfig
	{
		Util::Color::Rgba color;
		float			  sensitivity;
		uint			  thickness;
	};

	namespace GraphicsConfigs
	{
		inline const OutlineConfig OUTLINE_DEFAULT { COLOR_OUTLINE_DEFAULT,
													 OUTLINE_SENSITIVITY_DEFAULT,
													 OUTLINE_THICKNESS_DEFAULT };
	} // namespace GraphicsConfigs
} // namespace VTX::Renderer

namespace VTX::Renderer::Builder::PostProcess
{
	struct Outline
	{
		inline static const Desc::Key PASS = "Outline";

		static Desc::Key build( GraphBuilder & p_graph, const Desc::Key & p_input )
		{
			p_graph.pass( PASS )
				.in( p_input )
				.in( "Depth" )
				.out( PASS )
				.program( PASS )
				.shaders( { "default.vert", "outline.frag" } )
				.uniform( "Color", COLOR_WHITE.toLinear() )
				.uniform(
					"Sensitivity",
					OUTLINE_SENSITIVITY_DEFAULT,
					std::pair { OUTLINE_SENSITIVITY_MIN, OUTLINE_SENSITIVITY_MAX }
				)
				.uniform(
					"Thickness", OUTLINE_THICKNESS_DEFAULT, std::pair { OUTLINE_THICKNESS_MIN, OUTLINE_THICKNESS_MAX }
				)
				.endProgram()
				.endPass();

			return PASS;
		}

		static void upload( Context::ContextWrapper & p_context, const OutlineConfig & p_config )
		{
			BinaryBuffer140 buffer;
			buffer.write( p_config.color.toLinear() );
			buffer.write( p_config.sensitivity );
			buffer.write( p_config.thickness );
			buffer.close();

			p_context.setBuffer( { PASS }, buffer );
		}
	};
} // namespace VTX::Renderer::Builder::PostProcess

#endif
