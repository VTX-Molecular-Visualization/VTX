#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/command_buffer.hpp"
#include "renderer/context/gl/buffer.hpp"
#include "renderer/context/gl/chrono.hpp"
#include "renderer/context/gl/framebuffer.hpp"
#include "renderer/context/gl/include_opengl.hpp"
#include "renderer/context/gl/program.hpp"
#include "renderer/context/gl/sampler.hpp"
#include "renderer/context/gl/struct_opengl_infos.hpp"
#include "renderer/context/gl/texture_2d.hpp"
#include "renderer/context/gl/vertex_array.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/resource_handler.hpp"
#include "renderer/struct_infos.hpp"

namespace VTX::Renderer::Context::Backend
{
	/**
	 * @brief OpenGL 4.5 backend.
	 */
	class OpenGL45
	{
	  public:
		/**
		 * @brief Texture binding info.
		 */
		struct TextureBinding
		{
			Desc::Handle  texture;
			Desc::Handle  sampler;
			Desc::Binding unit;
		};

		/**
		 * @brief Buffer binding info.
		 */
		struct BufferBinding
		{
			Desc::Handle			   buffer;
			Desc::E_SHADER_BUFFER_KIND kind;
			Desc::Binding			   binding;
			uint32_t				   offsetBytes = 0;
			uint32_t				   sizeBytes   = 0;
		};

		/**
		 * @brief Resource table per pass.
		 */
		struct ResourceTable
		{
			std::vector<TextureBinding> textures;
			std::vector<BufferBinding>	shaderBuffers;
		};

		/**
		 * @brief Default constructor.
		 */
		OpenGL45( const size_t, const size_t, const FilePath &, void * = nullptr );

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const Desc::RenderQueue &, const Desc::Resources &, CommandBuffer & );

		/**
		 * @brief Resize textures.
		 */
		void resize( const uint32_t, const uint32_t, const Desc::PassList &, const Desc::ResourceMap<Desc::Texture> & );

		/**
		 * @brief Set data to a shader buffer.
		 */
		void setShaderBufferData( const Desc::Key &, SpanBytes, const size_t );

		/**
		 * @brief Set data to a pipeline buffer.
		 */
		void setPipelineBufferData( const Desc::Key &, SpanBytes, const size_t );

		/**
		 * @brief Get texture data at a given pixel, or full texture.
		 */
		std::vector<std::byte> getTextureData(
			const Desc::Key &,
			std::optional<Desc::E_FORMAT>,
			std::optional<size_t>,
			std::optional<size_t>
		) const;

		/**
		 * @brief Set texture data.
		 */
		void setTextureData( const Desc::Key & p_key, SpanBytes );

		/**
		 * @brief Set render target.
		 */
		void setRenderTarget( const Desc::E_RENDER_TARGET );

		/**
		 * @brief Fill backend infos.
		 */
		void fillInfos( StructInfos & p_infos ) const;

		/**
		 * @brief Resources accessors.
		 */
		inline const OpenGL45::ResourceTable & resourceTable( const Desc::Handle p_handle ) const noexcept
		{
			return _resourceTables.get( p_handle );
		}

		inline const GL::Framebuffer & framebuffer( const Desc::Handle p_handle ) const noexcept
		{
			return _framebuffers.get( p_handle );
		}

		inline const GL::Texture2D & texture( const Desc::Handle p_handle ) const noexcept
		{
			return _textures.get( p_handle );
		}

		inline const GL::Sampler & sampler( const Desc::Handle p_handle ) const noexcept
		{
			return _samplers.get( p_handle );
		}

		inline const GL::Program & program( const Desc::Handle p_handle ) const noexcept
		{
			return _programs.get( p_handle );
		}

		inline const GL::Buffer & shaderBuffer( const Desc::Handle p_handle ) const noexcept
		{
			return _shaderBuffers.get( p_handle );
		}

		inline const GL::Buffer & vertexBuffer( const Desc::Handle p_handle ) const noexcept
		{
			return _vertexBuffers.get( p_handle );
		}

		inline const GL::Buffer & indexBuffer( const Desc::Handle p_handle ) const noexcept
		{
			return _indexBuffers.get( p_handle );
		}

		inline const GL::VertexArray & vertexArray( const Desc::Handle p_handle ) const noexcept
		{
			return _vertexArrays.get( p_handle );
		}

	  private:
		/**
		 * @brief Current width and height.
		 */
		uint32_t _width;
		uint32_t _height;

		/**
		 * @brief Render target (default framebuffer or offscreen).
		 */
		Desc::Handle _default;
		Desc::Handle _offscreen;
		Desc::Handle _target;

		/**
		 * @brief Shader path.
		 */
		const FilePath _shaderPath;

		/**
		 * @brief Global shader buffers and resource tables.
		 */
		using GlobalShaderBuffers = std::vector<BufferBinding>;
		GlobalShaderBuffers _globalShaderBuffers;

		/**
		 * @brief Resource pools.
		 */
		ResourceHandler<GL::Framebuffer>					 _framebuffers;
		ResourceHandler<ResourceTable>						 _resourceTables;
		ResourceHandler<GL::VertexArray, Desc::VertexLayout> _vertexArrays;
		ResourceHandler<GL::Buffer, Desc::BufferPipeline>	 _vertexBuffers;
		ResourceHandler<GL::Buffer, Desc::BufferPipeline>	 _indexBuffers;
		ResourceHandler<GL::Buffer, Desc::BufferShader>		 _shaderBuffers;
		ResourceHandler<GL::Texture2D, Desc::Texture>		 _textures;
		ResourceHandler<GL::Sampler, Desc::Sampler>			 _samplers;
		ResourceHandler<GL::Program, Desc::Program>			 _programs;

		/**
		 * @brief Get or create resources.
		 */
		Desc::Handle _getOrCreateQuad();
		Desc::Handle _getOrCreateFramebuffer( const Desc::Pass &, const Desc::Resources &, const bool = false );
		Desc::Handle _getOrCreateResourceTable( const Desc::Pass &, const Desc::Resources & );
		Desc::Handle _getOrCreateTexture( const Desc::Key &, const Desc::Texture & );
		Desc::Handle _getOrCreateSampler( const Desc::Key &, const Desc::Sampler & );
		Desc::Handle _getOrCreateVertexLayout( const Desc::Key &, const Desc::VertexLayout & );
		Desc::Handle _getOrCreateShaderBuffer( const Desc::BufferShader & );
		Desc::Handle _getOrCreateVertexBuffer( const Desc::Key &, const Desc::BufferPipeline & );
		Desc::Handle _getOrCreateIndexBuffer( const Desc::Key &, const Desc::BufferPipeline & );
		Desc::Handle _getOrCreateProgram( const Desc::Program & );

		/**
		 * @brief Build resources.
		 */
		GlobalShaderBuffers _buildGlobalShaderBuffers( const Desc::Resources & );
		ResourceTable		_buildResourceTableForPass( const Desc::Pass &, const Desc::Resources & );

		/**
		 * @brief Bind resources.
		 */
		void _attachTexturesToFramebuffer( const Desc::Pass &, const Desc::ResourceMap<Desc::Texture> & );
		void _bindGeometryToVao( const Desc::Key &, const Desc::Geometry &, const Desc::Resources & );

		/**
		 * @brief Create the screen quad.
		 */
		inline static const Desc::Key _QUAD		   = "Quad";
		inline static const Desc::Key _QUAD_VBO	   = _QUAD + ".Position";
		inline static const Desc::Key _DEFAULT_FBO = "Default";

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();
	};
} // namespace VTX::Renderer::Context::Backend

#endif
