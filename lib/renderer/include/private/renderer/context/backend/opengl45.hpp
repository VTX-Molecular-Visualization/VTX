#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__

#include "renderer/context/command_buffer.hpp"
#include "renderer/context/gl/buffer.hpp"
#include "renderer/context/gl/chrono.hpp"
#include "renderer/context/gl/framebuffer.hpp"
#include "renderer/context/gl/include_opengl.hpp"
#include "renderer/context/gl/program_manager.hpp"
#include "renderer/context/gl/sampler.hpp"
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
		 * @brief Bind a framebuffer.
		 */
		inline void bindFramebuffer( const Handle p_framebuffer ) const noexcept
		{
			if ( p_framebuffer == NO_HANDLE )
			{
				GL::Framebuffer::bindDefault();
			}
			else
			{
				assert( p_framebuffer < _framebuffers.size() );
				_framebuffers[ p_framebuffer ]->bind();
			}
		}

		/**
		 * @brief Unbind a framebuffer.
		 */
		inline void unbindFramebuffer( const Handle p_framebuffer ) const noexcept
		{
			if ( p_framebuffer == NO_HANDLE )
			{
				GL::Framebuffer::unbindDefault();
			}
			else
			{
				assert( p_framebuffer < _framebuffers.size() );
				_framebuffers[ p_framebuffer ]->unbind();
			}
		}

	  private:
		/**
		 * @brief Current width and height.
		 */
		uint32_t _width;
		uint32_t _height;

		/**
		 * @brief Buffer binding info.
		 */
		struct BufferBinding
		{
			Handle	 buffer;
			uint32_t offsetBytes = 0;
			uint32_t sizeBytes	 = 0;
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
		 * @brief Cache mapping.
		 */
		using Cache = std::unordered_map<Key, Handle>;

		/**
		 * @brief Mappint Key -> Handle.
		 */
		Cache _cacheTextures;
		Cache _cacheSamplers;
		Cache _cacheBuffers;
		Cache _cacheVertexBuffers;
		Cache _cacheIndexBuffers;
		Cache _cacheVertexStreams;
		Cache _cachePrograms;
		Cache _cacheResourceTables;
		Cache _cacheFramebuffers;

		/**
		 * @brief Resource pools.
		 * index = Handle
		 */
		std::vector<std::unique_ptr<ResourceTable>>	  _resourceTables;
		std::vector<std::unique_ptr<GL::VertexArray>> _vertexArrays;
		std::vector<std::unique_ptr<GL::Buffer>>	  _buffers;
		std::vector<std::unique_ptr<GL::Buffer>>	  _vertexBuffers;
		std::vector<std::unique_ptr<GL::Buffer>>	  _indexBuffers;
		std::vector<std::unique_ptr<GL::Framebuffer>> _framebuffers;
		std::vector<std::unique_ptr<GL::Texture2D>>	  _textures;
		std::vector<std::unique_ptr<GL::Sampler>>	  _samplers;
		std::unique_ptr<GL::ProgramManager>			  _programManager;
		std::vector<GL::Program *>					  _programs;

		/**
		 * @brief Get or create resources.
		 */
		Handle _getOrCreateFramebuffer( const Pass &, const Resources &, const bool = false );
		Handle _getOrCreateResourceTable( const Pass &, const Resources & );
		Handle _getOrCreateTexture( const Key &, const Texture & );
		Handle _getOrCreateSampler( const Key &, const Sampler & );
		Handle _getOrCreateVertexStream( const Key &, const VertexLayout & );
		Handle _getOrCreateBuffer( const Key &, const BufferLayout & );
		Handle _getOrCreateProgram( const Program & );

		void _bindGeometryToVao( const Handle, const VertexLayout &, const Geometry &, const bool );

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();
		static void APIENTRY  _debugMessageCallback(
			 const GLenum	p_source,
			 const GLenum	p_type,
			 const GLuint	p_id,
			 const GLenum	p_severity,
			 const GLsizei	p_length,
			 const GLchar * p_msg,
			 const void *	p_data
		 ) noexcept;
	};
} // namespace VTX::Renderer::Context::Backend

#endif
