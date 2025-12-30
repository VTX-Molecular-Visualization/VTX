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
		 * @brief Binding types.
		 */
		enum class E_BINDING_TYPE : uint8_t
		{
			BUFFER_UNIFORM,
			BUFFER_STORAGE
		};

		/**
		 * @brief Buffer binding info.
		 */
		struct BufferBinding
		{
			Handle		   buffer;
			uint32_t	   offsetBytes = 0;
			uint32_t	   sizeBytes   = 0;
			E_BINDING_TYPE type		   = E_BINDING_TYPE::BUFFER_UNIFORM;
		};

		/**
		 * @brief Resource table (MVP).
		 */
		struct ResourceTable
		{
			std::vector<Handle>		   textures;	   // index = unit
			std::vector<BufferBinding> uniformBuffers; // index = binding
			std::vector<BufferBinding> storageBuffers; // index = binding
		};

		/**
		 * @brief Cache entry.
		 */
		template<typename Desc>
		struct CacheEntry
		{
			Hash   hash	  = 0;
			Handle handle = 0;
		};

		/**
		 * @brief Cache mapping.
		 */
		template<typename Desc>
		struct Cache
		{
			std::unordered_map<Key, CacheEntry<Desc>> map;
		};

		/**
		 * @brief Resource pools.
		 */
		std::vector<ResourceTable>			_resourceTables;
		std::vector<GL::VertexArray>		_vertexArrays;
		std::vector<GL::Buffer>				_buffers;
		std::vector<GL::Framebuffer>		_framebuffers;
		std::vector<GL::Texture2D>			_textures;
		std::unique_ptr<GL::ProgramManager> _programManager;
		std::vector<GL::Program>			_programs;

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
