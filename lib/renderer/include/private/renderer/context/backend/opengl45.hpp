#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__

#include "renderer/context/command_buffer.hpp"
#include "renderer/context/gl/buffer.hpp"
#include "renderer/context/gl/chrono.hpp"
#include "renderer/context/gl/framebuffer.hpp"
#include "renderer/context/gl/include_opengl.hpp"
#include "renderer/context/gl/program_manager.hpp"
#include "renderer/context/gl/struct_opengl_infos.hpp"
#include "renderer/context/gl/texture_2d.hpp"
#include "renderer/context/gl/vertex_array.hpp"
#include "renderer/descriptors.hpp"

namespace VTX::Renderer::Context::Backend
{
	/**
	 * @brief OpenGL 4.5 backend.
	 */
	class OpenGL45
	{
	  public:
		/**
		 * @brief Default constructor.
		 */
		OpenGL45( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_proc = nullptr );

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const RenderQueue & p_renderQueue, const Resources & p_resources, CommandBuffer & p_commands );

		/**
		 * @brief Resize textures.
		 */
		void resize( const size_t p_width, const size_t p_height );

		/**
		 * @brief Set the output framebuffer handle.
		 */
		inline void setOutput( const Handle p_output ) { _output = p_output; }

	  private:
		/**
		 * @brief Output framebuffer handle.
		 */
		Handle _output;

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;

		void				 _getOpenglInfos();
		static void APIENTRY _debugMessageCallback(
			const GLenum   p_source,
			const GLenum   p_type,
			const GLuint   p_id,
			const GLenum   p_severity,
			const GLsizei  p_length,
			const GLchar * p_msg,
			const void *   p_data
		) noexcept;
	};
} // namespace VTX::Renderer::Context::Backend

#endif
