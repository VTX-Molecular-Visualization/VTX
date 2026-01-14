#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__

#include "renderer/binary_buffer.hpp"
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
			Handle	texture;
			Handle	sampler;
			Binding unit;
		};

		/**
		 * @brief Buffer binding info.
		 */
		struct BufferBinding
		{
			Handle				 buffer;
			E_SHADER_BUFFER_KIND kind;
			Binding				 binding;
			uint32_t			 offsetBytes = 0;
			uint32_t			 sizeBytes	 = 0;
		};

		/**
		 * @brief Resource table per pass.
		 */
		struct ResourceTable
		{
			std::vector<TextureBinding> textures;
			std::vector<BufferBinding>	shaderBuffers;
			// std::vector<BufferBinding>	pipelineBuffers;
		};

		/**
		 * @brief Default constructor.
		 */
		OpenGL45( const size_t, const size_t, const FilePath &, void * = nullptr );

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const RenderQueue &, const Resources &, CommandBuffer & );

		/**
		 * @brief Resize textures.
		 */
		void resize( const uint32_t, const uint32_t, const PassList &, const ResourceMap<Texture> & );

		/**
		 * @brief Set data to a shader buffer.
		 */
		void setShaderBufferData( const Key &, SpanBytes );

		/**
		 * @brief Set data to a pipeline buffer.
		 */
		void setPipelineBufferData( const Key &, SpanBytes );

		/**
		 * @brief Fill backend infos.
		 */
		void fillInfos( StructInfos & p_infos ) const;

		/**
		 * @brief Resource table accessors.
		 */
		inline const OpenGL45::ResourceTable & resourceTable( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _resourceTables.size() );
			return _resourceTables[ p_handle ];
		}

		/**
		 * @brief GL object accessors.
		 */
		inline const GL::Framebuffer & framebuffer( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _framebuffers.size() );
			return *_framebuffers[ p_handle ];
		}

		inline const GL::Texture2D & texture( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _textures.size() );
			return *_textures[ p_handle ];
		}

		inline const GL::Sampler & sampler( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _samplers.size() );
			return *_samplers[ p_handle ];
		}

		inline const GL::Program & program( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _programs.size() );
			return *_programs[ p_handle ];
		}

		inline const GL::Buffer & shaderBuffer( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _shaderBuffers.size() );
			return *_shaderBuffers[ p_handle ];
		}

		inline const GL::Buffer & vertexBuffer( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _vertexBuffers.size() );
			return *_vertexBuffers[ p_handle ];
		}

		inline const GL::Buffer & indexBuffer( const Handle p_handle ) const noexcept
		{
			assert( p_handle < _indexBuffers.size() );
			return *_indexBuffers[ p_handle ];
		}

		inline const GL::VertexArray & vertexArray( const Handle p_handle ) const noexcept
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
		 * @brief Global shader buffers.
		 */
		using GlobalShaderBuffers = std::vector<BufferBinding>;

		/**
		 * @brief Cache : mapping Key -> Handle.
		 */
		using Cache = std::unordered_map<Key, Handle>;
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
		GLObject<GL::VertexArray>			_vertexArrays;
		GLObject<GL::Buffer>				_shaderBuffers;
		GLObject<GL::Buffer>				_vertexBuffers;
		GLObject<GL::Buffer>				_indexBuffers;
		GLObject<GL::Framebuffer>			_framebuffers;
		GLObject<GL::Texture2D>				_textures;
		GLObject<GL::Sampler>				_samplers;
		std::unique_ptr<GL::ProgramManager> _programManager;
		std::vector<GL::Program *>			_programs;

		/**
		 * @brief Save buffer properties.
		 */
		struct _ShaderBufferCacheEntry
		{
			E_SHADER_BUFFER_KIND role;
			E_BUFFER_MUTABILITY	 mutability;
			E_BUFFER_ACCESS		 access;
			E_UPDATE_FREQUENCY	 frequency;
		};

		struct _PipelineBufferCacheEntry
		{
			E_PIPELINE_BUFFER_KIND kind;
			E_UPDATE_FREQUENCY	   frequency;
		};

		std::unordered_map<Key, _ShaderBufferCacheEntry>   _shaderBufferProperties;
		std::unordered_map<Key, _PipelineBufferCacheEntry> _pipelineBufferProperties;

		/**
		 * @brief Get or create resources.
		 */
		Handle _getOrCreateFramebuffer( const Pass &, const Resources &, const bool = false );
		Handle _getOrCreateResourceTable( const Pass &, const Resources & );
		Handle _getOrCreateTexture( const Key &, const Texture & );
		Handle _getOrCreateSampler( const Key &, const Sampler & );
		Handle _getOrCreateVertexLayout( const Key &, const VertexLayout & );
		Handle _getOrCreateShaderBuffer( const BufferShader & );
		Handle _getOrCreateVertexBuffer( const Key & );
		Handle _getOrCreateIndexBuffer( const Key & );
		Handle _getOrCreateProgram( const Program & );

		/**
		 * @brief Build resources.
		 */
		GlobalShaderBuffers _buildGlobalShaderBuffers( const Resources & );
		ResourceTable		_buildResourceTableForPass( const Pass &, const Resources & );

		/**
		 * @brief Bind resources.
		 */
		void _attachTexturesToFramebuffer( const Pass &, const ResourceMap<Texture> & );
		void _bindGeometryToVao( const Key &, const Geometry &, const Resources & );

		/**
		 * @brief Create the screen quad.
		 */
		inline static const Key _QUAD = "Quad";
		void					_createQuad();

		/**
		 * @brief Specs.
		 */
		GL::StructOpenglInfos _openglInfos;
		void				  _getOpenglInfos();
	};
} // namespace VTX::Renderer::Context::Backend

#endif
