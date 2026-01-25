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
#include "renderer/context/resource_handler.hpp"
#include "renderer/descriptors.hpp"
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
		void setShaderBufferData( const Desc::Key &, SpanBytes );

		/**
		 * @brief Set data to a pipeline buffer.
		 */
		void setPipelineBufferData( const Desc::Key &, SpanBytes );

		/**
		 * @brief Get texture data at a given pixel.
		 */
		std::vector<std::byte> getTextureData( const Desc::Key &, const size_t, const size_t ) const;

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
		ResourceHandler<ResourceTable>	 _resourceTables;
		ResourceHandler<GL::VertexArray> _vertexArrays;
		ResourceHandler<GL::Buffer>		 _shaderBuffers;
		ResourceHandler<GL::Buffer>		 _vertexBuffers;
		ResourceHandler<GL::Buffer>		 _indexBuffers;
		ResourceHandler<GL::Framebuffer> _framebuffers;
		ResourceHandler<GL::Texture2D>	 _textures;
		ResourceHandler<GL::Sampler>	 _samplers;
		ResourceHandler<GL::Program>	 _programs;

		/**
		 * @brief Save buffer and texture properties.
		 */
		struct _TextureProperties
		{
			Desc::E_FORMAT format;
			Desc::Size2D   size;
		};

		struct _ShaderBufferProperties
		{
			Desc::E_SHADER_BUFFER_KIND role;
			Desc::E_BUFFER_MUTABILITY  mutability;
			Desc::E_BUFFER_ACCESS	   access;
			Desc::E_UPDATE_FREQUENCY   frequency;
		};

		struct _PipelineBufferProperties
		{
			Desc::E_PIPELINE_BUFFER_KIND kind;
			Desc::E_UPDATE_FREQUENCY	 frequency;
		};

		template<typename T>
		using Properties = std::unordered_map<Desc::Handle, T>;
		Properties<_TextureProperties>		  _textureProperties;
		Properties<_ShaderBufferProperties>	  _shaderBufferProperties;
		Properties<_PipelineBufferProperties> _pipelineBufferProperties;

		/**
		 * @brief Get or create resources.
		 */
		Desc::Handle _getOrCreateFramebuffer( const Desc::Pass &, const Desc::Resources &, const bool = false );
		Desc::Handle _getOrCreateResourceTable( const Desc::Pass &, const Desc::Resources & );
		Desc::Handle _getOrCreateTexture( const Desc::Key &, const Desc::Texture & );
		Desc::Handle _getOrCreateSampler( const Desc::Key &, const Desc::Sampler & );
		Desc::Handle _getOrCreateVertexLayout( const Desc::Key &, const Desc::VertexLayout & );
		Desc::Handle _getOrCreateShaderBuffer( const Desc::BufferShader & );
		Desc::Handle _getOrCreateVertexBuffer( const Desc::Key & );
		Desc::Handle _getOrCreateIndexBuffer( const Desc::Key & );
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
		inline static const Desc::Key _QUAD		= "Quad";
		inline static const Desc::Key _QUAD_VBO = _QUAD + ".Position";
		void						  _createQuad();

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();
	};
} // namespace VTX::Renderer::Context::Backend

#endif
