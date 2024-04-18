#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
#include <any>
#include <concepts>
#include <util/chrono.hpp>

namespace VTX::Renderer::Context
{
	struct BaseContext
	{
		size_t	 width;
		size_t	 height;
		FilePath shaderPath;
		bool	 loaded = false;
	};

	template<typename C>
	concept Concept
		= std::is_base_of<BaseContext, C>::value
		  && requires(
			  C								p_context,
			  const size_t					p_width,
			  const size_t					p_height,
			  const size_t					p_x,
			  const size_t					p_y,
			  const RenderQueue &			p_renderQueue,
			  const Links &					p_links,
			  const Handle					p_output,
			  const SharedUniforms &		p_uniforms,
			  const std::string &			p_key,
			  const std::any &				p_value,
			  const std::vector<std::any> & p_data,
			  const size_t					p_size,
			  const size_t					p_index,
			  const size_t					p_offset,
			  const Util::Chrono::Task &	p_task,
			  Instructions &				p_instructions,
			  InstructionsDurationRanges &	p_instructionsDurationRanges,
			  StructInfos &					p_infos,
			  std::vector<uchar> &			p_image,
			  const std::string &			p_pass,
			  const E_CHAN_OUT		p_channel,
			  std::any &					p_textureData

		  ) {
				 {
					 p_context.build(
						 p_renderQueue, p_links, p_output, p_uniforms, p_instructions, p_instructionsDurationRanges
					 )
				 } -> std::same_as<void>;
				 {
					 p_context.resize( p_renderQueue, p_width, p_height )
				 } -> std::same_as<void>;
				 {
					 p_context.setOutput( p_output )
				 } -> std::same_as<void>;
				 {
					 p_context.setValue( p_value, p_key, p_index )
				 } -> std::same_as<void>;
				 // TODO: templated type not deductible.
				 //{
				 //	 p_context.reserveData( p_size, p_key )
				 //} -> std::same_as<void>;
				 {
					 p_context.setData( p_data, p_key )
				 } -> std::same_as<void>;
				 {
					 p_context.setSubData( p_data, p_key, p_offset )
				 } -> std::same_as<void>;
				 {
					 p_context.fillInfos( p_infos )
				 } -> std::same_as<void>;
				 {
					 p_context.measureTaskDuration( p_task )
				 } -> std::same_as<float>;
				 {
					 p_context.compileShaders()
				 } -> std::same_as<void>;
				 {
					 p_context.snapshot( p_image, p_renderQueue, p_instructions, p_width, p_height )
				 } -> std::same_as<void>;
				 {
					 p_context.getTextureData( p_textureData, p_x, p_y, p_pass, p_channel )
				 } -> std::same_as<void>;
			 };

} // namespace VTX::Renderer::Context

#endif
