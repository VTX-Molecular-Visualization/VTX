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
		 * @brief Resource table accessors.
		 */
		inline const OpenGL45::ResourceTable & resourceTable( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _resourceTables.size() );
			return _resourceTables[ p_handle ];
		}

		/**
		 * @brief GL object accessors.
		 */
		inline const GL::Framebuffer & framebuffer( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _framebuffers.size() );
			return *_framebuffers[ p_handle ];
		}

		inline const GL::Texture2D & texture( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _textures.size() );
			return *_textures[ p_handle ];
		}

		inline const GL::Sampler & sampler( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _samplers.size() );
			return *_samplers[ p_handle ];
		}

		inline const GL::Program & program( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _programs.size() );
			return *_programs[ p_handle ];
		}

		inline const GL::Buffer & shaderBuffer( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _shaderBuffers.size() );
			return *_shaderBuffers[ p_handle ];
		}

		inline const GL::Buffer & vertexBuffer( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _vertexBuffers.size() );
			return *_vertexBuffers[ p_handle ];
		}

		inline const GL::Buffer & indexBuffer( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _indexBuffers.size() );
			return *_indexBuffers[ p_handle ];
		}

		inline const GL::VertexArray & vertexArray( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _vertexArrays.size() );
			return *_vertexArrays[ p_handle ];
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
		 * @brief Global shader buffers.
		 */
		using GlobalShaderBuffers = std::vector<BufferBinding>;

		/**
		 * @brief Cache : mapping Key -> Handle.
		 */
		using Cache = std::unordered_map<Desc::Key, Desc::Handle>;
		Cache _cacheTextures;
		Cache _cacheSamplers;
		Cache _cacheShaderBuffers;
		Cache _cacheVertexBuffers;
		Cache _cacheIndexBuffers;
		Cache _cacheVertexLayouts;
		Cache _cachePrograms;
		Cache _cacheResourceTables;
		Cache _cacheFramebuffers;

		/**
		 * @brief Global shader buffers and resource tables.
		 */
		GlobalShaderBuffers		   _globalShaderBuffers;
		std::vector<ResourceTable> _resourceTables;

		/**
		 * @brief GL resource pools : index = Handle.
		 */
		template<typename T>
		using GLObject = std::vector<std::unique_ptr<T>>;
		GLObject<GL::VertexArray> _vertexArrays;
		GLObject<GL::Buffer>	  _shaderBuffers;
		GLObject<GL::Buffer>	  _vertexBuffers;
		GLObject<GL::Buffer>	  _indexBuffers;
		GLObject<GL::Framebuffer> _framebuffers;
		GLObject<GL::Texture2D>	  _textures;
		GLObject<GL::Sampler>	  _samplers;
		GLObject<GL::Program>	  _programs;

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
		inline static const Desc::Key _QUAD = "Quad";
		void						  _createQuad();

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();
	};
} // namespace VTX::Renderer::Context::Backend

#endif
