#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include "renderer/descriptors.hpp"
#include <any>
#include <concepts>

namespace VTX::Renderer::Context
{
	struct BaseContext
	{
		size_t	 width;
		size_t	 height;
		FilePath shaderPath;
	};

	template<typename C>
	concept Concept = std::is_base_of<BaseContext, C>::value
					  && requires(
						  C								p_context,
						  const size_t					p_width,
						  const size_t					p_height,
						  const RenderQueue &			p_renderQueue,
						  const Links &					p_links,
						  const Handle					p_output,
						  const Uniforms &				p_uniforms,
						  const std::string &			p_uniformKey,
						  const std::vector<std::any> & p_data,
						  UniformValue &				p_uniformValue,
						  Instructions &				p_instructions
					  ) {
							 {
								 p_context.build( p_renderQueue, p_links, p_output, p_uniforms, p_instructions )
							 } -> std::same_as<void>;
							 {
								 p_context.resize( p_renderQueue, p_width, p_height )
							 } -> std::same_as<void>;
							 {
								 p_context.setUniform( p_uniformValue, p_uniformKey )
							 } -> std::same_as<void>;
							 {
								 p_context.getUniform( p_uniformValue, p_uniformKey )
							 } -> std::same_as<void>;
							 {
								 p_context.setData( p_data, p_uniformKey )
							 } -> std::same_as<void>;
						 };

} // namespace VTX::Renderer::Context

#endif
